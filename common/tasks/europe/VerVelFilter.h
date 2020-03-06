/*
 * VerVelFilter.h
 *
 *  Created on: Feb 8, 2019
 *      Author: mx
 */

#ifndef TASKS_VERVELFILTER_H_
#define TASKS_VERVELFILTER_H_

#include "../RobotTask.h"
#include "../../data/Filter_VerVel_status.h"
#include "../../generic/RobotMath.h"
#include "../../generic/matrix.h"

class VerVelFilter:public RobotTask
{
	private:
		Matrix P,Q,R,H,K,OMEGA,A,B;
		Matrix state,measure,in;

		bool start=true;

		const double convergenceThr=10.0;

		DataAccess<Filter_VerVel_status>* filter_VerVel_access;
		DataAccess<NGC_status>* ngc_access;
		DataAccess<Time_status>* time_access;

	public:
		VerVelFilter(const char *name, DataAccess<Filter_VerVel_status>& Filter_VerVel_access, DataAccess<NGC_status>& NGC_access, DataAccess<Time_status>& Time_access) :RobotTask(name),
			P(1,1), Q(1,1), R(1,1), H(1,1), K(1,1), OMEGA(1,1), A(1,1), B(1,1), state(1,1), measure(1,1), in(1,1)
		{
			filter_VerVel_access = &Filter_VerVel_access;
			ngc_access = &NGC_access;
			time_access = &Time_access;

			read_config();
		}


		virtual ~VerVelFilter(){}


		virtual void execute()
		{
			Filter_VerVel_status task_status;
			task_status= filter_VerVel_access->get();

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
				state(0,0)=ngc_status.velocity_body.raw.w.value;

				start=false;
			}

			double f=filter();



			if (f<convergenceThr)
			{
				Filter_VerVel_status task_status;
				task_status = filter_VerVel_access->get();
				task_status.execution=TASK_RUNNING;
				filter_VerVel_access->set(task_status);

				start=true;
			}
		}


		virtual void compute()
		{
			double f=filter();

			if (f>=convergenceThr)
			{
				Filter_VerVel_status task_status;
				task_status = filter_VerVel_access->get();
				task_status.execution=TASK_INIT;
				filter_VerVel_access->set(task_status);

				start=true;
			}
		}


		double filter()
		{
			Filter_VerVel_status task_status;
			task_status = filter_VerVel_access->get();
			double q_w= task_status.q_w;
			double r_w= task_status.r_w;
			double Kw= task_status.Kw;
			double bw= task_status.bw;
			double gw= task_status.gw;


			NGC_status ngc_status;
			ngc_status=ngc_access->get();
			Variable wRaw=ngc_status.velocity_body.raw.w;
			double fwRef=ngc_status.force.actual.fw.value;


			Time_status time_status;
			time_status=time_access->get();
			double dt=time_status.dt;


			Q.eye();
			Q(0,0)=q_w;


			R.eye();
			R(0,0)=r_w;


			double h=0.0;
			if (wRaw.valid) h=1.0;


			if ( fabs(wRaw.value)>5.0*P(0,0) ) h=0.0;

			H.zeros();
			H(0,0)=h;


			OMEGA=H*P*H.t()+R;
			K=P*H.t()*OMEGA.inv();


			measure.zeros();
			if (h==1.0)
			{
				measure(0,0)=wRaw.value;
			}

			state=state + K*(measure-H*state);

			P=P-K*H*P;


			ngc_status.velocity_body.filtered.w.value=state(0,0);// + state(2,0);
			ngc_status.velocity_body.filtered.w.std=P(0,0);
			ngc_status.velocity_body.filtered.w.timeStamp=time_status.timeStamp;
			ngc_status.velocity_body.filtered.w.valid=true;


			double phi=ngc_status.pose.actual.phi.value;
			double theta=ngc_status.pose.actual.theta.value;
			double w=state(0,0);
			double zDot=w*cos(phi)*cos(theta);


			ngc_status.velocity_inertial.filtered.zDot.value=zDot;
			ngc_status.velocity_inertial.filtered.zDot.std=P(0,0);
			ngc_status.velocity_inertial.filtered.zDot.timeStamp=time_status.timeStamp;
			ngc_status.velocity_inertial.filtered.zDot.valid=true;



			A.zeros();
			A(0,0)=1.0 -Kw*dt;


			B.zeros();
			B(0,0)=bw*dt;


			in(0,0)=fwRef;

			Matrix bias(1,1);
			bias(0,0)=gw;

			state=A*state + B*in + bias;
			P=A*P*A.t()+Q;

			ngc_access->set(ngc_status);

			//printf("%lf       %lf   %lf      %lf   %lf\n",P.trace(),state(0,0),state(1,0),state(2,0), state(3,0));


			return P.trace();
		}


		virtual void setStatus(TaskStatus ts)
		{
			Filter_VerVel_status task_status;
			task_status = filter_VerVel_access->get();
			task_status.execution=ts;
			filter_VerVel_access->set(task_status);
		}


		virtual TaskStatus getStatus()
		{
			Filter_VerVel_status task_status;
			task_status = filter_VerVel_access->get();
			return task_status.execution;
		}


		virtual void set_param(int param_code,double val)
		{
			Filter_VerVel_status task_status;
			task_status = filter_VerVel_access->get();

			switch (param_code)
			{
				case 0: task_status.q_w=val; break;
				case 1: task_status.r_w=val; break;
				case 2: task_status.Kw=val; break;
				case 3: task_status.bw=val; break;
				case 4: task_status.gw=val; break;
			};

			filter_VerVel_access->set(task_status);

			write_config();
		}



		int read_config()
		{
			FILE *f;
			char name_dir_file[256];
			char value[100],value2[100];
			double val;

			Filter_VerVel_status task_status;
			task_status = filter_VerVel_access->get();

			sprintf(name_dir_file,"%s%s",CONFIGURATION_FILES_DIRECTORY,"VerVelFilter.cfg");

			if((f=fopen(name_dir_file,"r"))!=NULL)
			{
				while(!feof(f))
				{
					fscanf(f,"%s %s %lf\n",value,value2,&val);

					if (strcmp(value,"q_w")==0) task_status.q_w=val;
					if (strcmp(value,"r_w")==0) task_status.r_w=val;
					if (strcmp(value,"Kw")==0) task_status.Kw=val;
					if (strcmp(value,"bw")==0) task_status.bw=val;
					if (strcmp(value,"gw")==0) task_status.gw=val;

				}

				fclose(f);

				filter_VerVel_access->set(task_status);

				return 0;
			}

			else
			{
				printf("VerVelFilter::read_config - File error: %s\n",name_dir_file);
				return -1;
			}
		}



		int write_config()
		{
			FILE *f;
			char name_dir_file[256];

			Filter_VerVel_status task_status;
			task_status = filter_VerVel_access->get();

			sprintf(name_dir_file,"%s%s",CONFIGURATION_FILES_DIRECTORY,"VerVelFilter.cfg");

			if((f=fopen(name_dir_file,"w"))!=NULL)
			{
				fprintf(f,"q_w = %lf\n", task_status.q_w );
				fprintf(f,"r_w = %lf\n", task_status.r_w );
				fprintf(f,"Kw = %lf\n", task_status.Kw );
				fprintf(f,"bw = %lf\n", task_status.bw );
				fprintf(f,"gw = %lf\n", task_status.gw );

				fclose(f);

				return 0;
			}

			else
			{
				printf("VerVelFilter::write_config - File error: %s\n",name_dir_file);
				return -1;
			}
		}
};


#endif /* TASKS_VERVELFILTER_H_ */
