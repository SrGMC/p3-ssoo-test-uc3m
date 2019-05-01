#include <string.h>
#include "include/concurrency_layer.h"

void init_tests(int * exit, pthread_mutex_t * exit_mutex);
void stop(int * exit, pthread_mutex_t * exit_mutex);
void destroy_tests(pthread_mutex_t * exit_mutex, stock_market * market);
int run_test(int index);
int random_number_between_range(int min_n, int max_n);

/* By default runs a standar test (test 1). 
 * To run other tests, execute concurrent_market with a number as argument 
 * Avalilable number of test are from 1 to 9. Described in memoria.pdf
 * Returns 0 if execution was successfull and 1 otherwise 
 * batch_files/ and stocks.txt should be in the same path as concurrent_market*/
int main(int argc, char * argv[]){
	if (argc == 1) {
		run_test(1);
		return 0;
	}
	int index = strtol(argv[1], NULL, 10);
	if (run_test(index)){
		return 1;
	} else {
		return 0;
	}
}

/* 
 * Initialize test variables
 * exit: Pointer to the exit variable
 * exit_mutex: Pointer to the exit mutex
 * market: Pointer to an uninitialized market structure
 */
void init_tests(int * exit, pthread_mutex_t * exit_mutex){
	init_concurrency_mechanisms();
	pthread_mutex_init(exit_mutex, NULL);
	*exit = 0;
}

/* 
 * Send signal to stop all operation_executer and stats_reader threads
 * exit: Pointer to the exit variable
 * exit_mutex: Pointer to the exit mutex
 */
void stop(int * exit, pthread_mutex_t * exit_mutex){
	pthread_mutex_lock(exit_mutex);
	*exit = 1;
	pthread_mutex_unlock(exit_mutex);
}

/* 
 * Destroy test variables
 * exit: Pointer to the exit variable
 * exit_mutex: Pointer to the exit mutex
 * market: Pointer to a market structure
 */
void destroy_tests(pthread_mutex_t * exit_mutex, stock_market * market){
	delete_market(market);
	destroy_concurrency_mechanisms();
	pthread_mutex_destroy(exit_mutex);
}

/*
 * Run index test
 * Total available tests: from 1 to 8
 * Returns: 0 if test ran correctly, -1 if there was an error
 */
int run_test(int index){
	/* Init test variables */
	int exit;
	void * op_res;
	int result = 0;
	pthread_mutex_t exit_mutex;
	stock_market market;
	init_market(&market, "stocks.txt");
	init_tests(&exit, &exit_mutex);

	/* Create op_exec */
	pthread_t op_thid;
	exec_info op_info;
	op_info.market = &market;
	op_info.exit = &exit;
	op_info.exit_mutex = &exit_mutex;

	pthread_create(&op_thid, NULL, &operation_executer, (void*) &op_info);

	if(index == 1){
		/* Create one broker */
		pthread_t br_thid;
		broker_info br_info1; strcpy(br_info1.batch_file, "batch_files/batch_op3.txt"); br_info1.market = &market;

		pthread_create(&br_thid, NULL, &broker, (void*) &br_info1);

		/* Wait for broker finalization */
		void * res;
		pthread_join(br_thid, &res);

		if(*(int*)(res) != 0){ result = 1; }

		/* End the execution of op_exec */
		stop(&exit, &exit_mutex);

	} else if(index == 2){
		/* Create two simultaneous brokers */
		pthread_t br_thid[2];
		/* Both brokers have more than 10 operations (queue maximum)*/
		broker_info br_info1; strcpy(br_info1.batch_file, "batch_files/batch_op4.txt"); br_info1.market = &market;
		broker_info br_info2; strcpy(br_info2.batch_file, "batch_files/batch_op9.txt"); br_info2.market = &market;

		pthread_create(&br_thid[0], NULL, &broker, (void*) &br_info1);
		pthread_create(&br_thid[1], NULL, &broker, (void*) &br_info2);

		/* Wait for broker finalization */
		void * res1;
		void * res2;
		pthread_join(br_thid[0], &res1);
		pthread_join(br_thid[1], &res2);

		if(*(int*)(res1) != 0){ result = 1; }
		if(*(int*)(res2) != 0){ result = 1; }

		/* End the execution of op_exec */
		stop(&exit, &exit_mutex);

	} else if(index == 3){
		/* Create two brokers */
		pthread_t br_thid[2];
		/* Both brokers have more than 10 operations (queue maximum)*/
		broker_info br_info1; strcpy(br_info1.batch_file, "batch_files/batch_op4.txt"); br_info1.market = &market;
		broker_info br_info2; strcpy(br_info2.batch_file, "batch_files/batch_op9.txt"); br_info2.market = &market;

		pthread_create(&br_thid[0], NULL, &broker, (void*) &br_info1);

		/* Wait for the first broker to finish */
		void * res1;
		pthread_join(br_thid[0], &res1);

		/* Simulate a wait */
		sleep(2);

		pthread_create(&br_thid[1], NULL, &broker, (void*) &br_info2);

		/* Wait for the second broker to finish */
		void * res2;
		pthread_join(br_thid[1], &res2);

		if(*(int*)(res1) != 0){ result = 1; }
		if(*(int*)(res2) != 0){ result = 1; }

		/* End the execution of op_exec */
		stop(&exit, &exit_mutex);

	} else if(index == 4){
		/* Create two simultaneous brokers */
		pthread_t br_thid[2];
		/* Both brokers have less than 10 operations (queue maximum)*/
		broker_info br_info1; strcpy(br_info1.batch_file, "batch_files/batch_op1.txt"); br_info1.market = &market;
		broker_info br_info2; strcpy(br_info2.batch_file, "batch_files/batch_op10.txt"); br_info2.market = &market;

		pthread_create(&br_thid[0], NULL, &broker, (void*) &br_info1);
		pthread_create(&br_thid[1], NULL, &broker, (void*) &br_info2);

		/* Wait for broker finalization */
		void * res1;
		void * res2;
		pthread_join(br_thid[0], &res1);
		pthread_join(br_thid[1], &res2);

		if(*(int*)(res1) != 0){ result = 1; }
		if(*(int*)(res2) != 0){ result = 1; }

		/* End the execution of op_exec */
		stop(&exit, &exit_mutex);

	} else if(index == 5){
		/* Create three simultaneous brokers */
		pthread_t br_thid[3];
		broker_info br_info1; strcpy(br_info1.batch_file, "batch_files/batch_op2.txt"); br_info1.market = &market;
		broker_info br_info2; strcpy(br_info2.batch_file, "batch_files/batch_op7.txt"); br_info2.market = &market;
		broker_info br_info3; strcpy(br_info3.batch_file, "batch_files/batch_op9.txt"); br_info3.market = &market;

		pthread_create(&br_thid[0], NULL, &broker, (void*) &br_info1);
		pthread_create(&br_thid[1], NULL, &broker, (void*) &br_info2);
		pthread_create(&br_thid[2], NULL, &broker, (void*) &br_info3);

		/* Wait for broker finalization */
		void * res1;
		void * res2;
		void * res3;
		pthread_join(br_thid[0], &res1);
		pthread_join(br_thid[1], &res2);
		pthread_join(br_thid[2], &res3);

		if(*(int*)(res1) != 0){ result = 1; }
		if(*(int*)(res2) != 0){ result = 1; }
		if(*(int*)(res3) != 0){ result = 1; }

		/* End the execution of op_exec */
		stop(&exit, &exit_mutex);

	} else if(index == 6){
		/* Create one broker */
		pthread_t br_thid;
		/* This batch file has an error */
		broker_info br_info1; strcpy(br_info1.batch_file, "batch_files/batch_inv.txt"); br_info1.market = &market;

		pthread_create(&br_thid, NULL, &broker, (void*) &br_info1);

		/* Wait for broker finalization */
		void * res1;
		pthread_join(br_thid, &res1);

		if(*(int*)(res1) != 0){ result = 1; }

		/* End the execution of op_exec */
		stop(&exit, &exit_mutex);

	} else if(index == 7){
		/* Create one broker and one reader */
		pthread_t br_thid;
		pthread_t re_thid;
		broker_info br_info1; strcpy(br_info1.batch_file, "batch_files/batch_op3.txt"); br_info1.market = &market;
		reader_info re_info1; re_info1.exit = &exit; re_info1.exit_mutex = &exit_mutex; re_info1.market = &market;
			re_info1.frequency = random_number_between_range(500000, 5000000);

		pthread_create(&re_thid, NULL, &stats_reader, (void*) &re_info1);

		/* Simulate a wait */
		sleep(1);

		pthread_create(&br_thid, NULL, &broker, (void*) &br_info1);

		/* Wait for broker finalization */
		void * res1;
		pthread_join(br_thid, &res1);

		if(*(int*)(res1) != 0){ result = 1; }

		/* End the execution of op_exec and reader */
		stop(&exit, &exit_mutex);

		/* Wait for reader finalization */
		void * res2;
		pthread_join(re_thid, &res2);

		if(*(int*)(res2) != 0){ result = 1; }

	}  else if(index == 8){
		/* Create three brokers and three readers at different times */
		pthread_t br_thid[3];
		pthread_t re_thid[3];
		broker_info br_info1; strcpy(br_info1.batch_file, "batch_files/batch_op2.txt"); br_info1.market = &market;
		broker_info br_info2; strcpy(br_info2.batch_file, "batch_files/batch_op7.txt"); br_info2.market = &market;
		broker_info br_info3; strcpy(br_info3.batch_file, "batch_files/batch_op9.txt"); br_info3.market = &market;
		reader_info re_info1; re_info1.exit = &exit; re_info1.exit_mutex = &exit_mutex; re_info1.market = &market;
			re_info1.frequency = random_number_between_range(500000, 5000000);
		reader_info re_info2; re_info2.exit = &exit; re_info2.exit_mutex = &exit_mutex; re_info2.market = &market;
			re_info2.frequency = random_number_between_range(500000, 5000000);
		reader_info re_info3; re_info3.exit = &exit; re_info3.exit_mutex = &exit_mutex; re_info3.market = &market;
			re_info3.frequency = random_number_between_range(500000, 5000000);

		pthread_create(&re_thid[0], NULL, &stats_reader, (void*) &re_info1);
		pthread_create(&re_thid[1], NULL, &stats_reader, (void*) &re_info2);

		pthread_create(&br_thid[0], NULL, &broker, (void*) &br_info1);
		pthread_create(&br_thid[1], NULL, &broker, (void*) &br_info2);

		/* Wait for brokers finalization */
		void * br_res1;
		void * br_res2;
		pthread_join(br_thid[0], &br_res1);
		pthread_join(br_thid[1], &br_res2);
		if(*(int*)(br_res1) != 0){ result = 1; }
		if(*(int*)(br_res2) != 0){ result = 1; }

		/* Another broker and reader join the market */
		pthread_create(&re_thid[2], NULL, &stats_reader, (void*) &re_info3);
		pthread_create(&br_thid[2], NULL, &broker, (void*) &br_info3);

		/* Wait for broker finalization */

		void * br_res3;
		pthread_join(br_thid[2], &br_res3);
		if(*(int*)(br_res3) != 0){ result = 1; }

		/* End the execution of op_exec and reader */
		stop(&exit, &exit_mutex);

		/* Wait for reader finalization */
		void * re_res1;
		void * re_res2;
		void * re_res3;
		pthread_join(re_thid[0], &re_res1);
		pthread_join(re_thid[1], &re_res2);
		pthread_join(re_thid[2], &re_res3);
		if(*(int*)(re_res1) != 0){ result = 1; }
		if(*(int*)(re_res2) != 0){ result = 1; }
		if(*(int*)(re_res3) != 0){ result = 1; }

	} else if(index == 9){
		/* Create one broker */
		pthread_t br_thid;
		/* This batch file does not exist */
		broker_info br_info1; strcpy(br_info1.batch_file, "doIevenExist.txt"); br_info1.market = &market;

		pthread_create(&br_thid, NULL, &broker, (void*) &br_info1);

		/* Wait for broker finalization */
		void * res1;
		pthread_join(br_thid, &res1);

		if(*(int*)(res1) != 1){ result = 1; }

		/* End the execution of op_exec */
		stop(&exit, &exit_mutex);

	} else {
		/* End the execution of op_exec */
		stop(&exit, &exit_mutex);

		result = 1;
	}
	
	/* Wait for op_exec finalization */
	pthread_join(op_thid, &op_res);
	if(*(int*)(op_res) != 0){ result = 1; }

	/* Print market statistics */
	print_market_status(&market);

	/* Destroy test initialization */
	destroy_tests(&exit_mutex, &market);
	return result;
}

/*
 * Return a random number between min_n and max_n, both inclusive
 */
int random_number_between_range(int min_n, int max_n) {
    return rand() % (max_n - min_n + 1) + min_n;
}
