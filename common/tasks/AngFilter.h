/*
 * angFilter.h
 *
 *  Created on: Feb 8, 2019
 *      Author: mx
 */

#ifndef TASKS_ANGFILTER_H_
#define TASKS_ANGFILTER_H_


#include "RobotTask.h"
#include "../data/Filter_Ang_status.h"
#include "../generic/RobotMath.h"
#include "../generic/matrix.h"


class AngFilter:public RobotTask
{
	private:
		Matrix P,Q,R,H,K,OMEGA,A,B;
		Matrix state,measure,in;

		bool start=true;

		const double convergenceThr=10.0;

		DataAccess<Filter_Ang_status>* filter_Ang_access;
		DataAccess<NGC_status>* ngc_access;
		DataAccess<Time_status>* time_access;

	public:
		AngFilter(const char *name, DataAccess<Filter_Ang_status>& Filter_Ang_access, DataAccess<NGC_status>& NGC_access, DataAccess<Time_status>& Time_access):RobotTask(name),
			P(2,2), Q(2,2), R(2,2), H(2,2), K(2,2), OMEGA(2,2), A(2,2), B(2,1), state(2,1), measure(2,1), in(1,1)
		{
			filter_Ang_access = &Filter_Ang_access;
			ngc_access = &NGC_access;
			time_access = &Time_access;
			
			read_config();
		}


		virtual ~AngFilter(){}


		virtual void execute()
		{
			Filter_Ang_status filter_Ang_status;
			filter_Ang_status=filter_Ang_access->get();

			if (filter_Ang_status.execution==TASK_INIT)
			{
				init();
			}
			if (filter_Ang_status.execution==TASK_RUNNING)
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
				state(0,0)=ngc_status.pose.raw.psi.value;
				state(1,0)=ngc_status.velocity_body.raw.r.value;

				start=false;
			}

			double f=filter();

			if (f<convergenceThr)
			{
				Filter_Ang_status filter_Ang_status;
				filter_Ang_status = filter_Ang_access->get();
				filter_Ang_status.execution=TASK_RUNNING;
				filter_Ang_access->set(filter_Ang_status);

				start=true;
			}
		}


		virtual void compute()
		{
			double f=filter();

			if (f>=convergenceThr)
			{
				Filter_Ang_status filter_Ang_status;
				filter_Ang_status = filter_Ang_access->get();
				filter_Ang_status.execution=TASK_INIT;
				filter_Ang_access->set(filter_Ang_status);

				start=true;
			}
		}



		double filter()
		{
			Filter_Ang_status filter_Ang_status;
			filter_Ang_status = filter_Ang_access->get();
			double q_psi=filter_Ang_status.q_psi;
			double q_r=filter_Ang_status.q_r;
			double r_psi=filter_Ang_status.r_psi;
			double r_r=filter_Ang_status.r_r;
			double Kr=filter_Ang_status.Kr;
			double br=filter_Ang_status.br;

			NGC_status ngc_status;
			ngc_status = ngc_access->get();
			Variable heading=ngc_status.pose.raw.psi;
			Variable yawRate=ngc_status.velocity_body.raw.r;
			double torqueRef=ngc_status.force.actual.tr.value;

			Time_status time_status;
			time_status=time_access->get();
			double dt=time_status.dt;


			Q.eye();
			Q(0,0)=q_psi;  Q(1,1)=q_r;

			R.eye();
			R(0,0)=r_psi;  R(1,1)=r_r;


			H.eye();

			if (!heading.valid) H(0,0)=0.0;
			if (!yawRate.valid) H(1,1)=0.0;


			OMEGA=H*P*H.t()+R;
			K=P*H.t()*OMEGA.inv();

			measure.zeros();
			measure(0,0)=heading.value; measure(1,0)=yawRate.value;

			state=state + K*(measure-H*state);

			P=P-K*H*P;


			ngc_status.pose.filtered.psi.value=state(0,0);
			ngc_status.pose.filtered.psi.std=P(0,0);
			ngc_status.pose.filtered.psi.timeStamp=time_status.timeStamp;
			ngc_status.pose.filtered.psi.valid=true;

			ngc_status.velocity_body.filtered.r.value=state(1,0);
			ngc_status.velocity_body.filtered.r.std=P(1,1);
			ngc_status.velocity_body.filtered.r.timeStamp=time_status.timeStamp;
			ngc_status.velocity_body.filtered.r.valid=true;

			ngc_status.pose.filtered.phi=ngc_status.pose.raw.phi;
			ngc_status.pose.filtered.theta=ngc_status.pose.raw.theta;
			ngc_status.velocity_body.filtered.p=ngc_status.velocity_body.raw.p;
			ngc_status.velocity_body.filtered.q=ngc_status.velocity_body.raw.q;



			//printf("psi: %lf    kf: %lf\n",heading.value,state(0,0));

			A.eye();
			A(0,1)=dt;	A(1,1)=1.0-Kr*dt;


			B.zeros();
			B(1,0)=dt*br*sign(torqueRef);

			//in(0,0)=torqueRef;
			in(0,0)=sqrt(fabs(torqueRef));

			state=A*state + B*in;
			state(0,0)=modpi(state(0,0));
			P=A*P*A.t()+Q;

			ngc_access->set(ngc_status);

			return P.trace();
		}



		virtual void setStatus(TaskStatus ts)
		{
			Filter_Ang_status filter_Ang_status;
			filter_Ang_status = filter_Ang_access->get();
			filter_Ang_status.execution=ts;
			filter_Ang_access->set(filter_Ang_status);
		}


		virtual TaskStatus getStatus()
		{
			Filter_Ang_status filter_Ang_status;
			filter_Ang_status = filter_Ang_access->get();
			return filter_Ang_status.execution;
		}


		virtual void set_param(int param_code,double val)
		{
			Filter_Ang_status filter_Ang_status;
			filter_Ang_status = filter_Ang_access->get();

			switch (param_code)
			{
				case 0: filter_Ang_status.q_psi=val; break;
				case 1: filter_Ang_status.q_r=val; break;
				case 2: filter_Ang_status.r_psi=val; break;
				case 3: filter_Ang_status.r_r=val; break;
				case 4: filter_Ang_status.Kr=val; break;
				case 5: filter_Ang_status.br=val; break;
			};

			filter_Ang_access->set(filter_Ang_status);

			write_config();
		}



		int read_config()
		{
			FILE *f;
			char name_dir_file[256];
			char value[100],value2[100];
			double val;

			Filter_Ang_status filter_Ang_status;
			filter_Ang_status = filter_Ang_access->get();
			
			sprintf(name_dir_file,"%s%s",CONFIGURATION_FILES_DIRECTORY,"AngFilter.cfg");
			
			if((f=fopen(name_dir_file,"r"))!=NULL)
			{
				while(!feof(f))
				{
					fscanf(f,"%s %s %lf\n",value,value2,&val);

					if (strcmp(value,"q_psi")==0) filter_Ang_status.q_psi=val;
					if (strcmp(value,"q_r")==0) filter_Ang_status.q_r=val;
					if (strcmp(value,"r_psi")==0) filter_Ang_status.r_psi=val;
					if (strcmp(value,"r_r")==0) filter_Ang_status.r_r=val;
					if (strcmp(value,"Kr")==0) filter_Ang_status.Kr=val;
					if (strcmp(value,"br")==0) filter_Ang_status.br=val;

				}

				fclose(f);

				filter_Ang_access->set(filter_Ang_status);

				return 0;
			}

			else
			{
				printf("AngFilter::read_config - File error: %s\n",name_dir_file);
				return -1;
			}
		}



		int write_config()
		{
			FILE *f;
			char name_dir_file[256];

			Filter_Ang_status filter_Ang_status;
			filter_Ang_status = filter_Ang_access->get();

			sprintf(name_dir_file,"%s%s",CONFIGURATION_FILES_DIRECTORY,"AngFilter.cfg");

			if((f=fopen(name_dir_file,"w"))!=NULL)
			{
				fprintf(f,"q_psi = %lf\n", filter_Ang_status.q_psi );
				fprintf(f,"q_r = %lf\n", filter_Ang_status.q_r );
				fprintf(f,"r_psi = %lf\n", filter_Ang_status.r_psi );
				fprintf(f,"r_r = %lf\n", filter_Ang_status.r_r );
				fprintf(f,"Kr = %lf\n", filter_Ang_status.Kr );
				fprintf(f,"br = %lf\n", filter_Ang_status.br );


				fclose(f);

				return 0;
			}

			else
			{
				printf("AngFilter::write_config - File error: %s\n",name_dir_file);
				return -1;
			}
		}
};


#endif /* TASKS_ANGFILTER_H_ */
