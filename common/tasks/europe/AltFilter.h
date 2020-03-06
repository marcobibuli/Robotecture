/*
 * AltFilter.h
 *
 *  Created on: Feb 8, 2019
 *      Author: mx
 */

#ifndef TASKS_ALTFILTER_H_
#define TASKS_ALTFILTER_H_


#include "../RobotTask.h"
#include "../../generic/RobotMath.h"
#include "../../generic/matrix.h"
#include "../../data/Filter_Alt_status.h"


class AltFilter:public RobotTask
{
	private:
		Matrix P,Q,R,H,K,OMEGA,A,B;
		Matrix state,measure,in;

		bool start=true;

		const double convergenceThr=10.0;

		DataAccess<Filter_Alt_status>* filter_Alt_access;
		DataAccess<NGC_status>* ngc_access;
		DataAccess<Time_status>* time_access;

	public:
		AltFilter(const char *name, DataAccess<Filter_Alt_status>& Filter_Alt_access, DataAccess<NGC_status>& NGC_access, DataAccess<Time_status>& Time_access) :RobotTask(name),
			P(1,1), Q(1,1), R(1,1), H(1,1), K(1,1), OMEGA(1,1), A(1,1), B(1,1), state(1,1), measure(1,1), in(1,1)
		{
			filter_Alt_access = &Filter_Alt_access;
			ngc_access = &NGC_access;
			time_access = &Time_access;

			read_config();
		}


		virtual ~AltFilter(){}


		virtual void execute()
		{
			Filter_Alt_status task_status;
			task_status=filter_Alt_access->get();

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
				ngc_status=ngc_access->get();
				state(0,0)=ngc_status.pose.raw.z.value;

				start=false;
			}

			double f=filter();



			if (f<convergenceThr)
			{
				Filter_Alt_status task_status;
				task_status = filter_Alt_access->get();
				task_status.execution=TASK_RUNNING;
				filter_Alt_access->set(task_status);

				start=true;
			}
		}


		virtual void compute()
		{
			double f=filter();

			if (f>=convergenceThr)
			{
				Filter_Alt_status task_status;
				task_status = filter_Alt_access->get();
				task_status.execution=TASK_INIT;
				filter_Alt_access->set(task_status);

				start=true;
			}
		}


		double filter()
		{
			Filter_Alt_status task_status;
			task_status = filter_Alt_access->get();
			double q_alt= task_status.q_alt;
			double r_alt= task_status.r_alt;


			NGC_status ngc_status;
			ngc_status=ngc_access->get();
			Variable altRaw=ngc_status.pose.raw.altitude;
			double zDot=ngc_status.velocity_inertial.actual.zDot.value;


			Time_status time_status;
			time_status=time_access->get();
			double dt=time_status.dt;


			Q.eye();
			Q(0,0)=q_alt;


			R.eye();
			R(0,0)=r_alt;


			double h=0.0;
			if (altRaw.valid) h=1.0;


			if ( fabs(altRaw.value)>5.0*P(0,0) ) h=0.0;

			H.zeros();
			H(0,0)=h;


			OMEGA=H*P*H.t()+R;
			K=P*H.t()*OMEGA.inv();


			measure.zeros();
			if (h==1.0)
			{
				measure(0,0)=altRaw.value;
			}

			state=state + K*(measure-H*state);

			P=P-K*H*P;


			ngc_status.pose.filtered.z.value=state(0,0);
			ngc_status.pose.filtered.z.std=P(0,0);
			ngc_status.pose.filtered.z.timeStamp=time_status.timeStamp;
			ngc_status.pose.filtered.z.valid=true;


			A.eye();


			B.zeros();
			B(0,0)=-dt;


			in(0,0)=zDot;



			state=A*state + B*in;
			P=A*P*A.t()+Q;

			ngc_access->set(ngc_status);

			//printf("%lf       %lf   %lf      %lf   %lf\n",P.trace(),state(0,0),state(1,0),state(2,0), state(3,0));


			return P.trace();
		}


		virtual void setStatus(TaskStatus ts)
		{
			Filter_Alt_status task_status;
			task_status=filter_Alt_access->get();
			task_status.execution=ts;
			filter_Alt_access->set(task_status);
		}


		virtual TaskStatus getStatus()
		{
			Filter_Alt_status task_status;
			task_status = filter_Alt_access->get();
			return task_status.execution;
		}


		virtual void set_param(int param_code,double val)
		{
			Filter_Alt_status task_status;
			task_status = filter_Alt_access->get();

			switch (param_code)
			{
				case 0: task_status.q_alt=val; break;
				case 1: task_status.r_alt=val; break;
			};

			filter_Alt_access->set(task_status);

			write_config();
		}



		int read_config()
		{
			FILE *f;
			char name_dir_file[256];
			char value[100],value2[100];
			double val;

			Filter_Alt_status task_status;
			task_status = filter_Alt_access->get();

			sprintf(name_dir_file,"%s%s",CONFIGURATION_FILES_DIRECTORY,"AltFilter.cfg");

			if((f=fopen(name_dir_file,"r"))!=NULL)
			{
				while(!feof(f))
				{
					fscanf(f,"%s %s %lf\n",value,value2,&val);

					if (strcmp(value,"q_alt")==0) task_status.q_alt=val;
					if (strcmp(value,"r_alt")==0) task_status.r_alt=val;

				}

				fclose(f);

				filter_Alt_access->set(task_status);

				return 0;
			}

			else
			{
				printf("AltFilter::read_config - File error: %s\n",name_dir_file);
				return -1;
			}
		}



		int write_config()
		{
			FILE *f;
			char name_dir_file[256];

			Filter_Alt_status task_status;
			task_status = filter_Alt_access->get();

			sprintf(name_dir_file,"%s%s",CONFIGURATION_FILES_DIRECTORY,"AltFilter.cfg");

			if((f=fopen(name_dir_file,"w"))!=NULL)
			{
				fprintf(f,"q_alt = %lf\n", task_status.q_alt );
				fprintf(f,"r_alt = %lf\n", task_status.r_alt );


				fclose(f);

				return 0;
			}

			else
			{
				printf("AltFilter::write_config - File error: %s\n",name_dir_file);
				return -1;
			}
		}
};



#endif /* TASKS_ALTFILTER_H_ */
