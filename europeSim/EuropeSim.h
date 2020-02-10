/*
 * SimStatus.h
 *
 *  Created on: Feb 06, 2020
 *      Author: mx
 */

#ifndef EUROPESIM_H_
#define EUROPESIM_H_


#include "../common/drivers/sim/SimStatus.h"



class EuropeSim:public SimStatus
{
	private:
		double ang_motor;
		Matrix versor;
		Matrix pos;
		double a_pos,a_neg,b_pos,b_neg;
		Matrix force;
		

	public:
		EuropeSim(const char *name);
		~EuropeSim();

		virtual void execute();
		
		

		void computeForceTorque(Matrix v);
		double thrust_curve(double v);

		int Svfl, Svfr, Svrl, Svrr, Shfl, Shfr, Shrl, Shrr;
		double Vvfl, Vvfr, Vvrl, Vvrr, Vhfl, Vhfr, Vhrl, Vhrr;

		long port_bow_thr_power, port_stern_thr_power, stbd_bow_thr_power, stbd_stern_thr_power;
		long port_bow_thr_enable, port_stern_thr_enable, stbd_bow_thr_enable, stbd_stern_thr_enable;

		long port_bow_azm_power, port_stern_azm_power, stbd_bow_azm_power, stbd_stern_azm_power;
		long port_bow_azm_enable, port_stern_azm_enable, stbd_bow_azm_enable, stbd_stern_azm_enable;

		long port_bow_thr_set, port_stern_thr_set, stbd_bow_thr_set, stbd_stern_thr_set;
		long port_bow_azm_set, port_stern_azm_set, stbd_bow_azm_set, stbd_stern_azm_set;

		double alpha_port_bow;
		double alpha_port_stern;
		double alpha_stbd_bow;
		double alpha_stbd_stern;
};



#endif /* EUROPESIM_H_ */
