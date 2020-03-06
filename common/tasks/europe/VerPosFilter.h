/*
 * VerPosFilter.h
 *
 *  Created on: Feb 8, 2019
 *      Author: mx
 */

#ifndef TASKS_VERPOSFILTER_H_
#define TASKS_VERPOSFILTER_H_

#include "../RobotTask.h"
#include "../../data/Filter_VerPos_status.h"
#include "../../generic/RobotMath.h"
#include "../../generic/matrix.h"


class VerPosFilter:public RobotTask
{
	private:
		Matrix P,Q,R,H,K,OMEGA,A,B;
		Matrix state,measure,in;

		bool start=true;

		const double convergenceThr=10.0;

		DataAccess<Filter_VerPos_status>* filter_VerPos_access;
		DataAccess<NGC_status>* ngc_access;
		DataAccess<Time_status>* time_access;


	public:
		VerPosFilter(const char *name, DataAccess<Filter_VerPos_status>& Filter_VerPos_access, DataAccess<NGC_status>& NGC_access, DataAccess<Time_status>& Time_access) :RobotTask(name),
			P(1,1), Q(1,1), R(1,1), H(1,1), K(1,1), OMEGA(1,1), A(1,1), B(1,1), state(1,1), measure(1,1), in(1,1)
		{
			filter_VerPos_access = &Filter_VerPos_access;
			ngc_access = &NGC_access;
			time_access = &Time_access;

			read_config();
		}


		virtual ~VerPosFilter(){}


		virtual void execute()
		{
			Filter_VerPos_status task_status;
			task_status = filter_VerPos_access->get();

			if (task_status.execution==TASK_INIT)
			{
				init();
			}
			if (task_status.execution==TASK_RUNNING)
			{
				compute();
			}
		}


		virtual void init()
		{
			if (start)
			{
				P.eye();  P=P*3000.0;
				state.zeros();

				NGC_status ngc_status;
				ngc_status = ngc_access->get();
				state(0,0)=ngc_status.pose.raw.z.value;

				start=false;
			}

			double f=filter();



			if (f<convergenceThr)
			{
				Filter_VerPos_status task_status;
				task_status = filter_VerPos_access->get();
				task_status.execution=TASK_RUNNING;
				filter_VerPos_access->set(task_status);

				start=true;
			}
		}


		virtual void compute()
		{
			double f=filter();

			if (f>=convergenceThr)
			{
				Filter_VerPos_status task_status;
				task_status = filter_VerPos_access->get();
				task_status.execution=TASK_INIT;
				filter_VerPos_access->set(task_status);

				start=true;
			}
		}


		double filter()
		{
			Filter_VerPos_status task_status;
			task_status = filter_VerPos_access->get();
			double q_z= task_status.q_z;
			double r_z= task_status.r_z;


			NGC_status ngc_status;
			ngc_status=ngc_access->get();
			Variable zRaw=ngc_status.pose.raw.z;
			double zDot=ngc_status.velocity_inertial.actual.zDot.value;


			Time_status time_status;
			time_status=time_access->get();
			double dt=time_status.dt;


			Q.eye();
			Q(0,0)=q_z;


			R.eye();
			R(0,0)=r_z;


			double h=0.0;
			if (zRaw.valid) h=1.0;


			if ( fabs(zRaw.value)>5.0*P(0,0) ) h=0.0;

			H.zeros();
			H(0,0)=h;


			OMEGA=H*P*H.t()+R;
			K=P*H.t()*OMEGA.inv();


			measure.zeros();
			if (h==1.0)
			{
				measure(0,0)=zRaw.value;
			}

			state=state + K*(measure-H*state);

			P=P-K*H*P;


			ngc_status.pose.filtered.z.value=state(0,0);
			ngc_status.pose.filtered.z.std=P(0,0);
			ngc_status.pose.filtered.z.timeStamp=time_status.timeStamp;
			ngc_status.pose.filtered.z.valid=true;


			A.eye();


			B.zeros();
			B(0,0)=dt;


			in(0,0)=zDot;



			state=A*state + B*in;
			P=A*P*A.t()+Q;

			ngc_access->set(ngc_status);

			//printf("%lf       %lf   %lf      %lf   %lf\n",P.trace(),state(0,0),state(1,0),state(2,0), state(3,0));


			return P.trace();
		}


		virtual void setStatus(TaskStatus ts)
		{
			Filter_VerPos_status task_status;
			task_status = filter_VerPos_access->get();
			task_status.execution=ts;
			filter_VerPos_access->set(task_status);
		}


		virtual TaskStatus getStatus()
		{
			Filter_VerPos_status task_status;
			task_status = filter_VerPos_access->get();
			return task_status.execution;
		}


		virtual void set_param(int param_code,double val)
		{
			Filter_VerPos_status task_status;
			task_status = filter_VerPos_access->get();

			switch (param_code)
			{
				case 0: task_status.q_z=val; break;
				case 1: task_status.r_z=val; break;
			};

			filter_VerPos_access->set(task_status);

			write_config();
		}



		int read_config()
		{
			FILE *f;
			char name_dir_file[256];
			char value[100],value2[100];
			double val;

			Filter_VerPos_status task_status;
			task_status = filter_VerPos_access->get();

			sprintf(name_dir_file,"%s%s",CONFIGURATION_FILES_DIRECTORY,"VerPosFilter.cfg");

			if((f=fopen(name_dir_file,"r"))!=NULL)
			{
				while(!feof(f))
				{
					fscanf(f,"%s %s %lf\n",value,value2,&val);

					if (strcmp(value,"q_z")==0) task_status.q_z=val;
					if (strcmp(value,"r_z")==0) task_status.r_z=val;

				}

				fclose(f);

				filter_VerPos_access->set(task_status);

				return 0;
			}

			else
			{
				printf("VerPosFilter::read_config - File error: %s\n",name_dir_file);
				return -1;
			}
		}



		int write_config()
		{
			FILE *f;
			char name_dir_file[256];

			Filter_VerPos_status task_status;
			task_status = filter_VerPos_access->get();

			sprintf(name_dir_file,"%s%s",CONFIGURATION_FILES_DIRECTORY,"VerPosFilter.cfg");

			if((f=fopen(name_dir_file,"w"))!=NULL)
			{
				fprintf(f,"q_z = %lf\n", task_status.q_z );
				fprintf(f,"r_z = %lf\n", task_status.r_z );


				fclose(f);

				return 0;
			}

			else
			{
				printf("VerPosFilter::write_config - File error: %s\n",name_dir_file);
				return -1;
			}
		}
};



#endif /* TASKS_VERPOSFILTER_H_ */
