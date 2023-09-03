#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h> /*build, using "gcc -o (filename) -pthread"*/

#define _CRT_SECURE_NO_WARNINGS
#define _(Text) (Text)

#define PAGESIZE (sysconf(_SC_PAGESIZE))

#define VMSTAT_FILE "/proc/vmstat"
#define STAT_FILE "/proc/stat"
#define MEMINFO_FILE "/proc/meminfo"

#define VMSTAT_BUFFSIZE 8192
#define STAT_CPU_BUFFSIZE 8192
#define STAT_BUFFSIZE 8192
#define MEMINFO_BUFFSIZE 8192


int pswpin_val = 0, pswpout_val = 0, pgpgin_val = 0, pgpgout_val = 0;
int pswpin_first = 0, pswpout_first = 0, pgpgin_first = 0, pgpgout_first = 0;
int pswpin_second = 0, pswpout_second = 0, pgpgin_second = 0, pgpgout_second = 0;
unsigned si_val = 0, bi_val = 0;
unsigned so_val = 0, bo_val = 0;
int MemFree_val =0; 
int Buffer_val =0;
int Cached_val =0; 
int SReclaimable_val =0;
int SwapTotal_val =0;
int SwapFree_val =0;
unsigned long free_val =0, buff_val=0, swpd_val=0, cache_val=0;
unsigned us_val = 0, sy_val = 0, id_val = 0, wa_val=0, st_val=0;
long long cpu_use = 0, cpu_sys = 0, cpu_idl = 0, cpu_iow = 0, cpu_sto = 0;
long long Div = 0, divo2 = 0;
unsigned long long stat_user = 0, stat_nice = 0, stat_sys = 0, stat_idle = 0, stat_iow = 0, stat_irq = 0, stat_srq = 0, stat_sto = 0;
int intr_val = 0;
int ctxt_val = 0;
int running_val = 0;
int blocked_val = 0;
int intr_first = 0, ctxt_first = 0;
int intr_second = 0, ctxt_second = 0;
int run_val = 0, block_val = 0;
unsigned long r_val, b_val;
unsigned in_val = 0, cs_val = 0;


static void new_format(void);

/********************************vmstat******************************************/

typedef struct vmstat_information { 

    char vmstat_data_first[30];
    int vmstat_val_first;

    char vmstat_data_second[30];
    int vmstat_val_second;

} vmstat_info;

void vmstat_select(vmstat_info* vmstat_data_read, int vm_size);

void vmstat_read() { //read vmstat file twice

	vmstat_info vmstat_data_read[300];
	int idx = 0;
	int i = 0;

	for (int vmstat_read = 0; vmstat_read < 2; vmstat_read++) {

		if (vmstat_read == 0) {

			FILE* fvmstat_first = fopen(VMSTAT_FILE, "r");
			if (fvmstat_first == NULL) {
				printf("fail to read file");
				return;
			}

			char vm_buffer[VMSTAT_BUFFSIZE], * vmstat_token;

			while (!feof(fvmstat_first)) {

				i = 0;
				fgets(vm_buffer, VMSTAT_BUFFSIZE, fvmstat_first);
				vmstat_token = strtok(vm_buffer, " ");
				while (vmstat_token != NULL) {

					if (i == 0) {
						strcpy(vmstat_data_read[idx].vmstat_data_first, vmstat_token);
					}
					else if (i == 1) {
						vmstat_data_read[idx].vmstat_val_first = atoi(vmstat_token);
					}
					i++;
					vmstat_token = strtok(NULL, " ");
				}
				idx++;
			}
			fclose(fvmstat_first);
			sleep(1); // 1s
		}

		else {

			FILE* fvmstat_second = fopen(VMSTAT_FILE, "r");
			if (fvmstat_second == NULL) {
				printf("fail to read VMSTAT file");
				return;
			}
			char vm_buffer[VMSTAT_BUFFSIZE], * vmstat_token;

			while (!feof(fvmstat_second)) {

				i = 0;
				fgets(vm_buffer, VMSTAT_BUFFSIZE, fvmstat_second);
				vmstat_token = strtok(vm_buffer, " ");
				while (vmstat_token != NULL) {

					if (i == 0) {
						strcpy(vmstat_data_read[idx].vmstat_data_second, vmstat_token);
					}
					else if (i == 1) {
						vmstat_data_read[idx].vmstat_val_second = atoi(vmstat_token);
					}
					i++;
					vmstat_token = strtok(NULL, " ");
				}
				idx++;
			}
			fclose(fvmstat_second);
		}
		
	}
	vmstat_select(vmstat_data_read, idx);
	return;
}


void vmstat_select(vmstat_info* vmstat_data_read, int vm_size) { //select necessary values

	int i = 0;
	unsigned long kb_per_page = sysconf(_SC_PAGESIZE) / 1024UL;

	for (i = 0; i < vm_size; i++) { 

		/*=====first====*/
		/*pswpin*/
		if (strstr(vmstat_data_read[i].vmstat_data_first, "pswpin") != NULL) {
			pswpin_first = vmstat_data_read[i].vmstat_val_first;
		}
		/*pswpout*/
		if (strstr(vmstat_data_read[i].vmstat_data_first, "pswpout") != NULL) {
			pswpout_first = vmstat_data_read[i].vmstat_val_first;
		}
		/*pgpgin*/
		if (strstr(vmstat_data_read[i].vmstat_data_first, "pgpgin") != NULL) {
			pgpgin_first = vmstat_data_read[i].vmstat_val_first;
		}
		/*pgpgout*/
		if (strstr(vmstat_data_read[i].vmstat_data_first, "pgpgout") != NULL) {
			pgpgout_first = vmstat_data_read[i].vmstat_val_first;
		}

		/*=====second====*/
		/*pswpin*/
		if (strstr(vmstat_data_read[i].vmstat_data_second, "pswpin") != NULL) {
			pswpin_second = vmstat_data_read[i].vmstat_val_second;
		}
		/*pswpout*/
		if (strstr(vmstat_data_read[i].vmstat_data_second, "pswpout") != NULL) {
			pswpout_second = vmstat_data_read[i].vmstat_val_second;
		}
		/*pgpgin*/
		if (strstr(vmstat_data_read[i].vmstat_data_second, "pgpgin") != NULL) {
			pgpgin_second = vmstat_data_read[i].vmstat_val_second;
		}
		/*pgpgout*/
		if (strstr(vmstat_data_read[i].vmstat_data_second, "pgpgout") != NULL) {
			pgpgout_second = vmstat_data_read[i].vmstat_val_second;
		}

	}

	pswpin_val = pswpin_second - pswpin_first;
	pswpout_val = pswpout_second - pswpout_first;
	pgpgin_val = pgpgin_second - pgpgin_first;
	pgpgout_val = pgpgout_second - pgpgout_first;

	/*si*/
	si_val = (unsigned)((unsigned long)(pswpin_val)*kb_per_page);
	/*so*/
	so_val = (unsigned)((unsigned long)(pswpout_val)*kb_per_page);
	/*bi*/
	bi_val = (unsigned)(pgpgin_val);
	/*bo*/
	bo_val = (unsigned)(pgpgout_val);

	return;
}

/***********************************meminfo********************************/

typedef struct meminfo_information {

	char meminfo_data[21];
	int meminfo_val;
	char meminfo_unit[4];

} meminfo_info;

void meminfo_select(meminfo_info* meminfo_data_read, int meminfo_size);

void meminfo_read() {

	meminfo_info meminfo_data_read[100];
	FILE* fmeminfo = fopen(MEMINFO_FILE, "r");

	if (fmeminfo == NULL) {
		printf("fail to read file");
		return;
	}

	char meminfo_buffer[MEMINFO_BUFFSIZE], * meminfo_token;

	int i = 0;
	int idx = 0;

	while (!feof(fmeminfo)) {

		i = 0;
		fgets(meminfo_buffer, MEMINFO_BUFFSIZE, fmeminfo);
		meminfo_token = strtok(meminfo_buffer, " ");
		while (meminfo_token != NULL) {

			if (i == 0) {
				strcpy(meminfo_data_read[idx].meminfo_data, meminfo_token);
			}
			else if (i == 1) {
				meminfo_data_read[idx].meminfo_val = atoi(meminfo_token);

			}
			i++;
			meminfo_token = strtok(NULL, " ");

		}
		idx++;
	}

	fclose(fmeminfo);
	meminfo_select(meminfo_data_read, idx);

	return;
}

void meminfo_select(meminfo_info* meminfo_data_read, int meminfo_size) {

	for (int i = 0; i < meminfo_size; i++) {

		/*free*/
		if (strstr(meminfo_data_read[i].meminfo_data, "MemFree") != NULL) {
			MemFree_val = meminfo_data_read[i].meminfo_val;
		}
		/*buffer*/
		if (strstr(meminfo_data_read[i].meminfo_data, "Buffers") != NULL) {
			Buffer_val = meminfo_data_read[i].meminfo_val;
		}
		/*cache = cached + sreclaimable*/
		if (strcmp(meminfo_data_read[i].meminfo_data, "Cached:") == 0) {
			Cached_val = meminfo_data_read[i].meminfo_val;
		}
		if (strstr(meminfo_data_read[i].meminfo_data, "SReclaimable") != NULL) {
			SReclaimable_val = meminfo_data_read[i].meminfo_val;
		}
		/*swpd = swaptotal - swapfree*/
		if (strstr(meminfo_data_read[i].meminfo_data, "SwapTotal") != NULL) {
			SwapTotal_val = meminfo_data_read[i].meminfo_val;
		}
		if (strstr(meminfo_data_read[i].meminfo_data, "SwapFree") != NULL) {
			SwapFree_val = meminfo_data_read[i].meminfo_val;	
		}

	}

	/*free*/
	free_val = (unsigned long)(MemFree_val);
	/*buff*/
	buff_val = (unsigned long)(Buffer_val);
	/*swpd*/
	swpd_val = (unsigned long)(SwapTotal_val - SwapFree_val);
	/*cache*/
	cache_val = (unsigned long)(Cached_val + SReclaimable_val);

	return;
}


/*****************************************stat cpu***********************************************/
typedef struct cpu_information {

	char cpu[10];
	int cpu_arry[10];

} cpu_info;

void cpu_select(cpu_info* cpu_data_read);

void cpu_read() {

	cpu_info cpu_data_read;
	FILE* fstat_cpu = fopen(STAT_FILE, "r");

	if (fstat_cpu == NULL) {
		printf("fail to read file");
		return;
	}
	char stat_cpu_buffer[STAT_CPU_BUFFSIZE], * line;

	fgets(stat_cpu_buffer, STAT_CPU_BUFFSIZE, fstat_cpu);

	line = strtok(stat_cpu_buffer, " ");
	strcpy(cpu_data_read.cpu, line);

	int i = 0;
	char* stat_cpu_box = NULL;
	stat_cpu_box = strtok(NULL, " ");

	while (stat_cpu_box != NULL) {

		cpu_data_read.cpu_arry[i] = atoi(stat_cpu_box); 
		stat_cpu_box = strtok(NULL, " "); 

		i++;
	}

	fclose(fstat_cpu);
	cpu_select(&cpu_data_read);

	return;
}

void cpu_select(cpu_info* cpu_data_read) {

	stat_user = cpu_data_read->cpu_arry[0];
	stat_nice = cpu_data_read->cpu_arry[1];
	stat_sys = cpu_data_read->cpu_arry[2];
	stat_idle = cpu_data_read->cpu_arry[3];
	stat_iow = cpu_data_read->cpu_arry[4];
	stat_irq = cpu_data_read->cpu_arry[5];
	stat_srq = cpu_data_read->cpu_arry[6];
	stat_sto = cpu_data_read->cpu_arry[7];

	/*if you have guest cpu, add here
	stat_gue = cpu_arry[8]
			and
	stat_gue_nice = cpu_arry[10]*/

	cpu_use = (long long)(stat_user + stat_nice);
	cpu_sys = (long long)(stat_sys + stat_irq + stat_srq);
	cpu_idl = (long long)(stat_idle);
	cpu_iow = (long long)(stat_iow);
	cpu_sto = (long long)(stat_sto);

	Div = cpu_use + cpu_sys + cpu_idl + cpu_iow + cpu_sto;
	if (!Div) Div = 1, cpu_idl = 1;
	divo2 = Div / 2UL;

	/* us */
	us_val = (unsigned)((100 * cpu_use + divo2) / Div);
	/* sy */
	sy_val = (unsigned)((100 * cpu_sys + divo2) / Div);
	/* id */
	id_val = (unsigned)((100 * cpu_idl + divo2) / Div);
	/* wa */
	wa_val = (unsigned)((100 * cpu_iow + divo2) / Div);
	/* st */
	st_val = (unsigned)((100 * cpu_sto + divo2) / Div);

	return;
}


/*******************************************stat***************************************/
typedef struct stat_informatioin {

	char stat_data_first[21];
	int stat_val_first; 

	char stat_data_second[21];
	int stat_val_second;

} stat_info;

void stat_select(stat_info* stat_data_read, int stat_size);

void stat_read() {

	stat_info stat_data_read[100];

	int i = 0;
	int idx = 0;

	for (int stat_read = 0; stat_read < 2; stat_read++) {

		if (stat_read == 0) {

			FILE* fstat = fopen(STAT_FILE, "r");

			if (fstat == NULL) {
				printf("fail to read file");
				return;
			}

			char stat_buffer[STAT_BUFFSIZE], * stat_token;

			while (!feof(fstat)) {

				i = 0;
				fgets(stat_buffer, STAT_BUFFSIZE, fstat);
				stat_token = strtok(stat_buffer, " ");

				while (stat_token != NULL) {

					if (i == 0) {
						strcpy(stat_data_read[idx].stat_data_first, stat_token);
					}
					else if (i == 1) {
						stat_data_read[idx].stat_val_first = atoi(stat_token);

					}
					i++;
					stat_token = strtok(NULL, " ");

				}
				idx++;
			}
			fclose(fstat);
			sleep(1);

		}

		else {

			FILE* fstat = fopen(STAT_FILE, "r");

			if (fstat == NULL) {
				printf("fail to read file");
				return;
			}

			char stat_buffer[STAT_BUFFSIZE], * stat_token;

			while (!feof(fstat)) {

				i = 0;
				fgets(stat_buffer, STAT_BUFFSIZE, fstat);
				stat_token = strtok(stat_buffer, " ");

				while (stat_token != NULL) {

					if (i == 0) {
						strcpy(stat_data_read[idx].stat_data_second, stat_token);
					}
					else if (i == 1) {
						stat_data_read[idx].stat_val_second = atoi(stat_token);
					}
					i++;
					stat_token = strtok(NULL, " ");
				}
				idx++;
			}
			fclose(fstat);
		}

	}
	stat_select(stat_data_read, idx);
	return;
}

void stat_select(stat_info* stat_data_read, int stat_size) { 

	int i = 0;

	for (i = 0; i < stat_size; i++) {
		/*************************first*********************************/
		/*intr*/
		if (strstr(stat_data_read[i].stat_data_first, "intr") != NULL) {
			intr_first = stat_data_read[i].stat_val_first;
		}
		/*ctxt*/
		if (strstr(stat_data_read[i].stat_data_first, "ctxt") != NULL) {
			ctxt_first = stat_data_read[i].stat_val_first;
		}
		/*****************************second*********************************/
		/*intr*/
		if (strstr(stat_data_read[i].stat_data_second, "intr") != NULL) {
			intr_second = stat_data_read[i].stat_val_second;
		}
		/*ctxt*/
		if (strstr(stat_data_read[i].stat_data_second, "ctxt") != NULL) {
			ctxt_second = stat_data_read[i].stat_val_second;
		}
		/*procs_running*/
		if (strstr(stat_data_read[i].stat_data_second, "procs_running") != NULL) {
			running_val = stat_data_read[i].stat_val_second;
		}
		/*procs_blocking*/
		if (strstr(stat_data_read[i].stat_data_second, "procs_blocked") != NULL) {
			blocked_val = stat_data_read[i].stat_val_second;
		}
	}

	/*r*/
	r_val = (unsigned long)(running_val);
	/*b*/
	b_val = (unsigned long)(blocked_val);
	/*in*/
	in_val = (unsigned)(intr_second - intr_first);
	/*cs*/
	cs_val = (unsigned)(ctxt_second - ctxt_first);

	return;
}

/******************************************print*********************************/
static void new_header(void)
{
	const char* header =
		_("---procs--- ---------------------memory---------------------- ---swap-- -----io---- --system-- ---------cpu-------");
	const char format[] =
		"%4s %4s %12s %12s %12s %12s %4s %4s %5s %5s %4s %4s %3s %3s %3s %3s %3s";

    printf("%s", header);
    
    printf("\n");
    
    printf(format,

            _("r"),

            _("b"),

            _("swpd"),

            _("free"),

            _("buff"),

            _("cache"),

            _("si"),

            _("so"),

            _("bi"),

            _("bo"),

            _("in"),

            _("cs"),

            _("us"),

            _("sy"),

            _("id"),

            _("wa"),

            _("st"));

    printf("\n");
}

static void new_format(void)
{
	unsigned long kb_per_page = sysconf(_SC_PAGESIZE) / 1024UL;
	const char format[] = "%4lu %4lu %12lu %12lu %12lu %12lu %4u %4u %5u %5u %4u %4u %3u %3u %3u %3u %3u"; // start line

     printf(format,
         /* r */
		 r_val,
         /* b */
		 b_val,
         /* swpd */
         swpd_val,
         /* free */
         free_val,
         /* buff */
         buff_val,
         /* cache */
         cache_val,
         /* si */
		 si_val,
		 /* so */
		 so_val,
         /* bi */
		 bi_val,
		 /* bo */
		 bo_val,
		 /* in */
         in_val,
		 /* cs */
		 cs_val,
         /* us */
         us_val,
         /* sy */
         sy_val,
         /* id */
         id_val,
         /* wa */
         wa_val,
         /* st */
         st_val
        );
    printf("\n");
}


void* vmstat_thread(void* arg) {

	vmstat_read();
}

void* stat_thread(void* arg) {

	stat_read();
}

void threading_vmstat_and_stat() { //because of 'sleep(1)'

	pthread_t thread_file;
	pthread_create(&thread_file, NULL, vmstat_thread, NULL);
	pthread_create(&thread_file, NULL, stat_thread, NULL);
	pthread_join(thread_file, NULL);

	return;
}

int main() 
{
	threading_vmstat_and_stat();
	meminfo_read();
	cpu_read();
	new_header();
	new_format();
} 