/*
 * HorPosFilter.h
 *
 *  Created on: Feb 7, 2019
 *      Author: mx
 */

#ifndef TASKS_HORPOSFILTER_H_
#define TASKS_HORPOSFILTER_H_




#include "../RobotTask.h"



class HorPosFilter:public RobotTask
{
	private:
		Matrix P,Q,R,H,K,OMEGA,A,B;
		Matrix state,measure,in;

		bool start=true;

		const double convergenceThr=50.0;

		DataAccess<Filter_HorPos_status>* filter_HorPos_access;
		DataAccess<NGC_status>* ngc_access;
		DataAccess<Time_status>* time_access;

	public:
		HorPosFilter(const char *name, DataAccess<Filter_HorPos_status>& Filter_HorPos_access, DataAccess<NGC_status>& NGC_access, DataAccess<Time_status>& Time_access) :RobotTask(name),
			P(2,2), Q(2,2), R(2,2), H(2,2), K(2,2), OMEGA(2,2), A(2,2), B(2,2), state(2,1), measure(2,1), in(2,1)
		{
			filter_HorPos_access = &Filter_HorPos_access;
			ngc_access = &NGC_access;
			time_access = &Time_access;

			read_config();
		}


		virtual ~HorPosFilter(){}


		virtual void execute()
		{
			Filter_HorPos_status task_status;
			task_status= filter_HorPos_access->get();

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
				state(0,0)=ngc_status.pose.raw.x.value;
				state(1,0)=ngc_status.pose.raw.y.value;

				start=false;
			}

			double f=filter();



			if (f<convergenceThr)
			{
				Filter_HorPos_status task_status;
				task_status = filter_HorPos_access->get();
				task_status.execution=TASK_RUNNING;
				filter_HorPos_access->set(task_status);

				start=true;
			}
		}


		virtual void compute()
		{
			double f=filter();

			if (f>=convergenceThr)
			{
				Filter_HorPos_status task_status;
				task_status = filter_HorPos_access->get();
				task_status.execution=TASK_INIT;
				filter_HorPos_access->set(task_status);

				start=true;
			}
		}


		double filter()
		{
			Filter_HorPos_status task_status;
			task_status = filter_HorPos_access->get();
			double q_xy= task_status.q_xy;
			double r_xy_11= task_status.r_xy_11;
			double r_xy_12= task_status.r_xy_12;
			double r_xy_22= task_status.r_xy_22;


			NGC_status ngc_status;
			ngc_status=ngc_access->get();
			Variable xRaw=ngc_status.pose.raw.x;
			Variable yRaw=ngc_status.pose.raw.y;
			double xDot=ngc_status.velocity_inertial.actual.xDot.value;
			double yDot=ngc_status.velocity_inertial.actual.yDot.value;


			Time_status time_status;
			time_status=time_access->get();
			double dt=time_status.dt;


			Q.eye();
			Q(0,0)=Q(1,1)=q_xy;


			R.eye();
			R(0,0)=r_xy_11;  R(0,1)=r_xy_12;
			R(1,0)=r_xy_12;  R(1,1)=r_xy_22;


			double h=0.0;
			if (xRaw.valid && yRaw.valid) h=1.0;

			if (fabs(xRaw.value)>10.0*P(0,0) || fabs(yRaw.value)>10.0*P(1,1)) h=0.0;

			H.zeros();
			H(0,0)=h; H(1,1)=h;


			OMEGA=H*P*H.t()+R;
			K=P*H.t()*OMEGA.inv();


			//printf("****** vel   %lf   %lf\n",P(0,0),P(1,1));

			measure.zeros();
			if (h==1.0)
			{
				measure(0,0)=xRaw.value;
				measure(1,0)=yRaw.value;
			}

			state=state + K*(measure-H*state);

			P=P-K*H*P;


			ngc_status.pose.filtered.x.value=state(0,0);
			ngc_status.pose.filtered.x.std=P(0,0);
			ngc_status.pose.filtered.x.timeStamp=time_status.timeStamp;
			ngc_status.pose.filtered.x.valid=true;

			ngc_status.pose.filtered.y.value=state(1,0);
			ngc_status.pose.filtered.y.std=P(1,1);
			ngc_status.pose.filtered.y.timeStamp=time_status.timeStamp;
			ngc_status.pose.filtered.y.valid=true;



			A.eye();

			B.zeros();
			B(0,0)=dt;	  B(1,1)=dt;


			in(0,0)=xDot;
			in(1,0)=yDot;


			state=A*state + B*in;
			P=A*P*A.t()+Q;

			ngc_access->set(ngc_status);

			//printf("%lf       %lf   %lf      %lf   %lf\n",P.trace(),state(0,0),state(1,0),state(2,0), state(3,0));

			return P.trace();
		}


		virtual void setStatus(TaskStatus ts)
		{
			Filter_HorPos_status task_status;
			task_status=filter_HorPos_access->get();
			task_status.execution=ts;
			filter_HorPos_access->set(task_status);
		}


		virtual TaskStatus getStatus()
		{
			Filter_HorPos_status task_status;
			task_status = filter_HorPos_access->get();
			return task_status.execution;
		}


		virtual void set_param(int param_code,double val)
		{
			Filter_HorPos_status task_status;
			task_status = filter_HorPos_access->get();

			switch (param_code)
			{
				case 0: task_status.q_xy=val; break;
				case 1: task_status.r_xy_11=val; break;
				case 2: task_status.r_xy_12=val; break;
				case 3: task_status.r_xy_22=val; break;
			};

			filter_HorPos_access->set(task_status);

			write_config();
		}



		int read_config()
		{
			FILE *f;
			char name_dir_file[256];
			char value[100],value2[100];
			double val;

			Filter_HorPos_status task_status;
			task_status = filter_HorPos_access->get();

			sprintf(name_dir_file,"%s%s",CONFIGURATION_FILES_DIRECTORY,"HorPosFilter.cfg");

			if((f=fopen(name_dir_file,"r"))!=NULL)
			{
				while(!feof(f))
				{
					fscanf(f,"%s %s %lf\n",value,value2,&val);

					if (strcmp(value,"q_xy")==0) task_status.q_xy=val;
					if (strcmp(value,"r_xy_11")==0) task_status.r_xy_11=val;
					if (strcmp(value,"r_xy_12")==0) task_status.r_xy_12=val;
					if (strcmp(value,"r_xy_22")==0) task_status.r_xy_22=val;

				}

				fclose(f);

				filter_HorPos_access->set(task_status);

				return 0;
			}

			else
			{
				printf("HorPosFilter::read_config - File error: %s\n",name_dir_file);
				return -1;
			}
		}



		int write_config()
		{
			FILE *f;
			char name_dir_file[256];
			char value[100],value2[100];
			double val;

			Filter_HorPos_status task_status;
			task_status = filter_HorPos_access->get();

			sprintf(name_dir_file,"%s%s",CONFIGURATION_FILES_DIRECTORY,"HorPosFilter.cfg");

			if((f=fopen(name_dir_file,"w"))!=NULL)
			{
				fprintf(f,"q_xy = %lf\n", task_status.q_xy );
				fprintf(f,"r_xy_11 = %lf\n", task_status.r_xy_11 );
				fprintf(f,"r_xy_12 = %lf\n", task_status.r_xy_12 );
				fprintf(f,"r_xy_22 = %lf\n", task_status.r_xy_22 );

				fclose(f);

				return 0;
			}

			else
			{
				printf("HorPosFilter::write_config - File error: %s\n",name_dir_file);
				return -1;
			}
		}
};




#endif /* TASKS_HORPOSFILTER_H_ */
