#pragma once
#include "Struct.h"
 

/*
We can't use SEH for manual map driver's

*/

namespace DetectHyp
{
	bool ProcIsIntel()
	{
		int cpuid[4]{ -1 };
		__cpuid(cpuid, 0);
		if (cpuid[2] == 'letn')
		{
			return TRUE;
		}
		return FALSE;
	}

	 bool compare_list_cpuid() 
	{
		//compare cpuid  list
		int  invalid_cpuid_list[4] = { -1 };
		int valid_cpuid_list[4] = { -1 };

		__cpuid(invalid_cpuid_list, 0x13371337);
		__cpuid(valid_cpuid_list, 0x40000000);

		if ((invalid_cpuid_list[0] != valid_cpuid_list[0]) ||
			(invalid_cpuid_list[1] != valid_cpuid_list[1]) ||
			(invalid_cpuid_list[2] != valid_cpuid_list[2]) ||
			(invalid_cpuid_list[3] != valid_cpuid_list[3]))
			return TRUE;

		return FALSE; 



	}

	 bool cpuid_is_hypervisor()
	{
		int cpuid[4] = { 0 };
		__cpuid(cpuid, 1);
		return ((cpuid[2] >> 31) & 1);
	}



	  bool   time_attack_rdtsc()
	{ 
		uint64_t avg = 0;
		int cpuInfo[4] = {};
		for (int i = 0; i < 2500; i++)
		{

			auto currentIrql = __readcr8(); 
			__writecr8(HIGH_LEVEL);

			_disable();
			auto tick1 = __readmsr(IA32_TIME_STAMP_COUNTER);
			__cpuid(cpuInfo, 0);// vm-exit
			auto tick2 = __readmsr(IA32_TIME_STAMP_COUNTER);
			_enable();

			__writecr8(currentIrql);

			avg += (tick2 - tick1);
		}
		avg /= 2500;  
		return (avg > 500) || (25 > avg) ; 
	}

	 
 


	 bool time_attack_MPERF()
	{
		 // Some hypervisor just return 0(like:VMware)

		int cpuid[4]{ -1 };
		uint64_t  avg{ 0 };
		for (int i = 0; i < 100; i++)
		{
			auto currentIrql = __readcr8();
			__writecr8(HIGH_LEVEL);

			_disable(); 
			auto tick1 = __readmsr(IA32_MPERF_MSR);
			__cpuid(cpuid, 0);//call vm-exit
			auto tick2 = __readmsr(IA32_MPERF_MSR);
			_enable();

			__writecr8(currentIrql);

			if ( (!tick1 && !tick2 ) || (tick1 == tick2))
			{
				return true;
			}

			avg += (tick2 - tick1);
		}
		avg /= 100;   
		return  (0x4ff < avg) || (0xc > avg);
	}
	


	 bool time_attack_APERF()
	{
		uint64_t avg{ 0 };
		int data[4]{ -1 };
		for (size_t i = 0; i < 100; i++)
		{
			auto currentIrql = __readcr8();
			__writecr8(HIGH_LEVEL);


			_disable();
			auto  tick1 = __readmsr(IA32_APERF_MSR);
			__cpuid(data, 0); //call vm-exit
			auto tick2 = __readmsr(IA32_APERF_MSR);
			_enable();

			__writecr8(currentIrql);

			if ( (!tick1 && !tick2) || (tick1 == tick2))
			{
				return true;
			}
			avg += (tick2 - tick1);
		}
		avg /= 100;   
		return  (0x4ff < avg) || (0x33 > avg);
	}
	
	 bool lbr_is_virtulazed()
	{
		if (ProcIsIntel())
		{
			auto current_value = __readmsr(MSR_DEBUGCTL);//safe current value
			__writemsr(MSR_DEBUGCTL, DEBUGCTL_LBR | DEBUGCTL_BTF);
			auto whatch_write = __readmsr(MSR_DEBUGCTL);
			__writemsr(MSR_DEBUGCTL, current_value);
			return (!(whatch_write & DEBUGCTL_LBR));
		}
		return FALSE;
	}
 
	 bool lbr_stask_is_virtulazed()
	{
		int cpuid[4]{ -1 };
		 auto currentLBR =	__readmsr(MSR_P6M_LBSTK_TOS); 
		 __cpuid(cpuid, 0);//call vm-exit
		 auto exitLBR =	__readmsr(MSR_P6M_LBSTK_TOS); 
		 return currentLBR != exitLBR;

	}

	
	
	 
	
}
