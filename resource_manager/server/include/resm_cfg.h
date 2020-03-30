/*
 * @copyright Copyright (c) 2016-2019 TOYOTA MOTOR CORPORATION.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/******************************************************************************
File name   : resm_cfg.h
******************************************************************************/
#ifndef RESOURCE_MANAGER_SERVER_INCLUDE_RESM_CFG_H_
#define RESOURCE_MANAGER_SERVER_INCLUDE_RESM_CFG_H_

/******************************************************************************
Configuration
******************************************************************************/
/*************************************************
 *  Delay time from process start to monitoring start (sec)
 *    def:60    max:-   min:0
 *      (0: stop timer)
 *************************************************/
#define MONITORING_START_DELAT_TIME (60)


/*************************************************
 *  Delay time until authentication service monitoring start (sec)
 *  	def:30  max:-   min:0
 *      (0: stop timer)
 *  MONITORING_START_DELAT_TIME > set to this value
 *************************************************/
#define EUA_MONITORING_START_DELAT_TIME	(30)


/*************************************************
 *  Delay from drop_caches process startup (sec)
 *    def:20    max:-   min:0
 *      (0: stop timer)
 *************************************************/
#define DROP_CACHES_START_DELAT_TIME  (20)


///*************************************************
// *  CPU load threshold (%)
// *    def:70    max:100   min:0
// *************************************************/
//#define CPU_LOAD_THRESHOLD    (70)
/*************************************************
 *  CPU load threshold(%:XX.X)
 *    threshold:875  max:1000  min:0
 *************************************************/
#define CPU_LOAD_THRESHOLD    (875)


/*************************************************
 * Number of CPU overload processes to be logged (rows)
 *  def:5   max:-   min:0
 *************************************************/
#define CPU_HIGH_LOAD_P_LOG_NUM   (5)


///*************************************************
// * Log output frequency during CPU overload continuation (sec)
// *  def:30    max:-   min:0
// *
// *  Specify as a multiple of the polling interval(WTC_CPU_INTERVAL) for CPU load monitoring
// *************************************************/
//#define CPU_HIGH_LOAD_LOG_FREQ    (30)
/*************************************************
 * Log output frequency during CPU overload continuation (sec)
 *  def:60    max:-   min:0
 *
 *  Specify as a multiple of the polling interval(WTC_CPU_INTERVAL) for CPU load monitoring
 *************************************************/
#define CPU_HIGH_LOAD_LOG_FREQ    (60)


/*************************************************
 * perf Profiling/Measurement Processes
 *  def:1   max:-   min:1
 *************************************************/
#define PERF_MAX_PROCS  (1)

/*************************************************
 * perf profiling/Output rows (functions)
 *  def:5   max:-   min:1
 *************************************************/
#define PERF_MAX_LINES  (5)

/*************************************************
 * perf pofiling/perf-record running time (sec)
 *  def:1   max:-   min:1
 *************************************************/
#define PERF_RECORD_SPAN  (1)

#define TOP_TIMEOUT (60)

#define RESET_SEC (60)


#define WTC_CPU_INTERVAL (6)

#define TASK_STAT_THRESHOLD (70)
#define FIFO_TIMER_LIMIT (6)
#define TSS_TIMER_LIMIT (60)

#define CPU_TASK_INIT (1)
#define CPU_TASK_SHOW_BF (2)
#define CPU_TASK_SHOW_AF (3)
#define FIFO_TASK_SHOW (4)
#define TSS_TASK_SHOW (5)
#define CPU_FIFO_TASK_GET_ID (6)
#define CPU_TSS_TASK_GET_ID (7)
#define CPU_FIFO_TASK_GET_OCCUPANCY (8)
#define CPU_TSS_TASK_GET_OCCUPANCY (9)

#define STATUS_IDOL (1)
#define STATUS_CHECK_CPU (2)
#define STATUS_WATCH_PROCESS (3)

#define TASK_STAT_RANK_NUM (5)


#endif  // RESOURCE_MANAGER_SERVER_INCLUDE_RESM_CFG_H_
