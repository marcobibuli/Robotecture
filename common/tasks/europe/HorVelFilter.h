/*
 * HorVelFilter.h
 *
 *  Created on: Feb 8, 2019
 *      Author: mx
 */

#ifndef TASKS_HORVELFILTER_H_
#define TASKS_HORVELFILTER_H_


#include "../RobotTask.h"




class HorVelFilter:public RobotTask
{
	private:
		Matrix P,Q,R,H,K,OMEGA,A,B;
		Matrix state,measure,in;

		bool start=true;

		const double convergenceThr=50.0;

		DataAccess<Filter_HorVel_status>* filter_HorVel_access;
		DataAccess<NGC_status>* ngc_access;
		DataAccess<Time_status>* time_access;

	public:
		HorVelFilter(const char* name, DataAccess<Filter_HorVel_status>& Filter_HorVel_access, DataAccess<NGC_status>& NGC_access, DataAccess<Time_status>& Time_access) :RobotTask(name),
			P(4,4), Q(4,4), R(2,2), H(2,4), K(4,2), OMEGA(2,2), A(4,4), B(4,2), state(4,1), measure(2,1), in(2,1)
		{
			filter_HorVel_access = &Filter_HorVel_access;
			ngc_access = &NGC_access;
			time_access = &Time_access;

			read_config();
		}


		virtual ~HorVelFilter(){}


		virtual void execute()
		{
			Filter_HorVel_status filter_HorVel_status;
			filter_HorVel_status = filter_HorVel_access->get();

			if (filter_HorVel_status.execution==TASK_INIT)
			{
				init();
			}
			if (filter_HorVel_status.execution==TASK_RUNNING)
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
				state(0,0)=ngc_status.velocity_body.raw.u.value;
				state(1,0)=ngc_status.velocity_body.raw.v.value;

				start=false;
			}

			double f=filter();



			if (f<convergenceThr)
			{
				Filter_HorVel_status filter_HorVel_status;
				filter_HorVel_status=filter_HorVel_access->get();
				filter_HorVel_status.execution=TASK_RUNNING;
				filter_HorVel_access->set(filter_HorVel_status);

				start=true;
			}
		}


		virtual void compute()
		{
			double f=filter();

			if (f>=convergenceThr)
			{
				Filter_HorVel_status filter_HorVel_status;
				filter_HorVel_status = filter_HorVel_access->get();
				filter_HorVel_status.execution=TASK_INIT;
				filter_HorVel_access->set(filter_HorVel_status);

				start=true;
			}
		}


		double filter()
		{
			Filter_HorVel_status filter_HorVel_status;
			filter_HorVel_status=filter_HorVel_access->get();
			double q_uv=filter_HorVel_status.q_uv;
			double q_current=filter_HorVel_status.q_current;
			double r_uv_11=filter_HorVel_status.r_uv_11;
			double r_uv_12=filter_HorVel_status.r_uv_12;
			double r_uv_22=filter_HorVel_status.r_uv_22;
			double Ku=filter_HorVel_status.Ku;
			double bu=filter_HorVel_status.bu;
			double Kv=filter_HorVel_status.Kv;
			double bv=filter_HorVel_status.bv;


			NGC_status ngc_status;
			ngc_status=ngc_access->get();
			Variable uRaw=ngc_status.velocity_body.raw.u;
			Variable vRaw=ngc_status.velocity_body.raw.v;
			double fuRef=ngc_status.force.actual.fu.value;
			double fvRef=ngc_status.force.actual.fv.value;


			Time_status time_status;
			time_status=time_access->get();
			double dt=time_status.dt;


			Q.eye();
			Q(0,0)=Q(1,1)=q_uv;
			Q(2,2)=Q(3,3)=q_current;

			R.eye();
			R(0,0)=r_uv_11;  R(0,1)=r_uv_12;
			R(1,0)=r_uv_12;  R(1,1)=r_uv_22;


			double h=0.0;
			if (uRaw.valid && vRaw.valid) h=1.0;



			if (fabs(uRaw.value)>5.0*P(0,0) || fabs(vRaw.value)>5.0*P(1,1)) h=0.0;

			H.zeros();
			H(0,0)=h; H(1,1)=h; H(0,2)=h; H(1,3)=h;


			OMEGA=H*P*H.t()+R;
			K=P*H.t()*OMEGA.inv();


			//printf("****** vel   %lf   %lf\n",P(0,0),P(1,1));

			measure.zeros();
			if (h==1.0)
			{
				measure(0,0)=uRaw.value;
				measure(1,0)=vRaw.value;
			}

			state=state + K*(measure-H*state);

			P=P-K*H*P;


			ngc_status.velocity_body.filtered.u.value=state(0,0);// + state(2,0);
			ngc_status.velocity_body.filtered.u.std=P(0,0);
			ngc_status.velocity_body.filtered.u.timeStamp=time_status.timeStamp;
			ngc_status.velocity_body.filtered.u.valid=true;

			ngc_status.velocity_body.filtered.v.value=state(1,0);// + state(3,0);
			ngc_status.velocity_body.filtered.v.std=P(1,1);
			ngc_status.velocity_body.filtered.v.timeStamp=time_status.timeStamp;
			ngc_status.velocity_body.filtered.v.valid=true;

			ngc_status.velocity_body.filtered.u_sea.value=state(2,0);
			ngc_status.velocity_body.filtered.u_sea.std=P(2,2);
			ngc_status.velocity_body.filtered.u_sea.timeStamp=time_status.timeStamp;
			ngc_status.velocity_body.filtered.u_sea.valid=true;

			ngc_status.velocity_body.filtered.v_sea.value=state(3,0);
			ngc_status.velocity_body.filtered.v_sea.std=P(3,3);
			ngc_status.velocity_body.filtered.v_sea.timeStamp=time_status.timeStamp;
			ngc_status.velocity_body.filtered.v_sea.valid=true;

			double psi=ngc_status.pose.actual.psi.value;
			double u=state(0,0);
			double v=state(1,0);
			double u_sea=state(2,0);
			double v_sea=state(3,0);
			double xDot=cos(psi)*(u+u_sea) -sin(psi)*(v+v_sea);
			double yDot=sin(psi)*(u+u_sea) +cos(psi)*(v+v_sea);
			double xDot_sea=cos(psi)*(u_sea) -sin(psi)*(v_sea);
			double yDot_sea=sin(psi)*(u_sea) +cos(psi)*(v_sea);
			double speed=sqrt(xDot*xDot + yDot*yDot);
			double course=atan2(yDot,xDot);

			ngc_status.velocity_inertial.filtered.xDot.value=xDot;
			ngc_status.velocity_inertial.filtered.yDot.value=yDot;
			ngc_status.velocity_inertial.filtered.xDot_sea.value=xDot_sea;
			ngc_status.velocity_inertial.filtered.yDot_sea.value=yDot_sea;
			ngc_status.velocity_inertial.filtered.speed.value=speed;
			ngc_status.velocity_inertial.filtered.course.value=course;

			ngc_status.velocity_inertial.filtered.xDot.std=P(0,0);
			ngc_status.velocity_inertial.filtered.yDot.std=P(1,1);
			ngc_status.velocity_inertial.filtered.xDot_sea.std=P(2,2);
			ngc_status.velocity_inertial.filtered.yDot_sea.std=P(3,3);
			ngc_status.velocity_inertial.filtered.speed.std=P(0,0);
			ngc_status.velocity_inertial.filtered.course.std=P(1,1);

			ngc_status.velocity_inertial.filtered.xDot.timeStamp=time_status.timeStamp;
			ngc_status.velocity_inertial.filtered.yDot.timeStamp=time_status.timeStamp;
			ngc_status.velocity_inertial.filtered.xDot_sea.timeStamp=time_status.timeStamp;
			ngc_status.velocity_inertial.filtered.yDot_sea.timeStamp=time_status.timeStamp;
			ngc_status.velocity_inertial.filtered.speed.timeStamp=time_status.timeStamp;
			ngc_status.velocity_inertial.filtered.course.timeStamp=time_status.timeStamp;

			ngc_status.velocity_inertial.filtered.xDot.valid=true;
			ngc_status.velocity_inertial.filtered.yDot.valid=true;
			ngc_status.velocity_inertial.filtered.xDot_sea.valid=true;
			ngc_status.velocity_inertial.filtered.yDot_sea.valid=true;
			ngc_status.velocity_inertial.filtered.speed.valid=true;
			ngc_status.velocity_inertial.filtered.course.valid=true;



			A.zeros();
			A(0,0)=1.0 -Ku*dt;   A(1,1)=1.0 -Kv*dt;
			A(2,2)=1.0; A(3,3)=1.0;


			B.zeros();
			B(0,0)=bu*dt;	  B(1,1)=bv*dt;


			in(0,0)=fuRef;
			in(1,0)=fvRef;


			state=A*state + B*in;
			P=A*P*A.t()+Q;

			ngc_access->set(ngc_status);

			//printf("%lf       %lf   %lf      %lf   %lf\n",P.trace(),state(0,0),state(1,0),state(2,0), state(3,0));


			return P.trace();
		}


		virtual void setStatus(TaskStatus ts)
		{
			Filter_HorVel_status filter_HorVel_status;
			filter_HorVel_status =filter_HorVel_access->get();
			filter_HorVel_status.execution=ts;
			filter_HorVel_access->set(filter_HorVel_status);
		}


		virtual TaskStatus getStatus()
		{
			Filter_HorVel_status filter_HorVel_status;
			filter_HorVel_status = filter_HorVel_access->get();
			return filter_HorVel_status.execution;
		}



		virtual void set_param(int param_code,double val)
		{
			Filter_HorVel_status filter_HorVel_status;
			filter_HorVel_status = filter_HorVel_access->get();

			switch (param_code)
			{
				case 0: filter_HorVel_status.q_uv=val; break;
				case 1: filter_HorVel_status.q_current=val; break;
				case 2: filter_HorVel_status.r_uv_11=val; break;
				case 3: filter_HorVel_status.r_uv_12=val; break;
				case 4: filter_HorVel_status.r_uv_22=val; break;
				case 5: filter_HorVel_status.Ku=val; break;
				case 6: filter_HorVel_status.bu=val; break;
				case 7: filter_HorVel_status.Kv=val; break;
				case 8: filter_HorVel_status.bv=val; break;
			};

			filter_HorVel_access->set(filter_HorVel_status);

			write_config();
		}



		int read_config()
		{
			FILE *f;
			char name_dir_file[256];
			char value[100],value2[100];
			double val;

			Filter_HorVel_status filter_HorVel_status;
			filter_HorVel_status = filter_HorVel_access->get();

			sprintf(name_dir_file,"%s%s",CONFIGURATION_FILES_DIRECTORY,"HorVelFilter.cfg");

			if((f=fopen(name_dir_file,"r"))!=NULL)
			{
				while(!feof(f))
				{
					fscanf(f,"%s %s %lf\n",value,value2,&val);

					if (strcmp(value,"q_uv")==0) filter_HorVel_status.q_uv=val;
					if (strcmp(value,"q_current")==0) filter_HorVel_status.q_current=val;
					if (strcmp(value,"r_uv_11")==0) filter_HorVel_status.r_uv_11=val;
					if (strcmp(value,"r_uv_12")==0) filter_HorVel_status.r_uv_12=val;
					if (strcmp(value,"r_uv_22")==0) filter_HorVel_status.r_uv_22=val;
					if (strcmp(value,"Ku")==0) filter_HorVel_status.Ku=val;
					if (strcmp(value,"bu")==0) filter_HorVel_status.bu=val;
					if (strcmp(value,"Kv")==0) filter_HorVel_status.Kv=val;
					if (strcmp(value,"bv")==0) filter_HorVel_status.bv=val;

				}

				fclose(f);

				filter_HorVel_access->set(filter_HorVel_status);

				return 0;
			}

			else
			{
				printf("HorVelFilter::read_config - File error: %s\n",name_dir_file);
				return -1;
			}
		}



		int write_config()
		{
			FILE *f;
			char name_dir_file[256];

			Filter_HorVel_status filter_HorVel_status;
			filter_HorVel_status = filter_HorVel_access->get();

			sprintf(name_dir_file,"%s%s",CONFIGURATION_FILES_DIRECTORY,"HorVelFilter.cfg");

			if((f=fopen(name_dir_file,"w"))!=NULL)
			{
				fprintf(f,"q_uv = %lf\n", filter_HorVel_status.q_uv );
				fprintf(f,"q_current = %lf\n", filter_HorVel_status.q_current );
				fprintf(f,"r_uv_11 = %lf\n", filter_HorVel_status.r_uv_11 );
				fprintf(f,"r_uv_12 = %lf\n", filter_HorVel_status.r_uv_12 );
				fprintf(f,"r_uv_22 = %lf\n", filter_HorVel_status.r_uv_22 );
				fprintf(f,"Ku = %lf\n", filter_HorVel_status.Ku );
				fprintf(f,"bu = %lf\n", filter_HorVel_status.bu );
				fprintf(f,"Kv = %lf\n", filter_HorVel_status.Kv );
				fprintf(f,"bv = %lf\n", filter_HorVel_status.bv );


				fclose(f);

				return 0;
			}

			else
			{
				printf("HorVelFilter::write_config - File error: %s\n",name_dir_file);
				return -1;
			}
		}
};


#endif /* TASKS_HORVELFILTER_H_ */
