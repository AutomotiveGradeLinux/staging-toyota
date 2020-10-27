/*
 * @copyright Copyright (c) 2016-2020 TOYOTA MOTOR CORPORATION.
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
/**
* @file MDev_GpsRollOver.cpp
*/

#include "MDev_GpsRollOver.h"

#define TIM_ROLOVR_CALCORCNT_DAYS (1024 * 7)  /*1024 weeks * 7 days */

#define TMT_OK           (1)             /* Definition indicating normal status */
#define TMT_NG           (0)             /* Definitions indicating abnormal status */
#define TMT_TRUE         (1)             /* Definitions indicating the status for which the condition is true */
#define TMT_FALSE          (0)             /* Definitions indicating a status in which the condition is false */

#define TIM_ROLOVR_LEAPYEARDAYS     (366)        /*Number of days in the leap year */
#define TIM_ROLOVR_NOTLEAPYEARDAYS  (365)        /*Year days in non-leap years */

static const WORD kMonth[2][12] = {
    {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},   /* Number of days per month(For non-leap year) */
    {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}    /* Number of days per month(For leap years) */
};
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * FUNCTION   : ChkLeapYear
 * Function     : Leap year determination processing
 * Feature Overview   : Determine whether it is a leap year
 * Input      : u_int16 year
 * Output     : None
 * Return value    : u_int32
            TMT_TRUE  Leap year
            TMT_FALSE Non-leap year
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static u_int32 ChkLeapYear(u_int16 year) {
  int32 leap_year;

  /* Leap year determination processing (select subscripts in the [LEAP YEAR] table)   *The processing that matches the time correction processing after time difference value addition/subtraction is used. */
  leap_year = TMT_FALSE;
  if ((year % 4) == 0) {
      leap_year = TMT_TRUE;
  }
  if ((year % 100) == 0) {
      leap_year = TMT_FALSE;
  }

  if ((year % 400) == 0) {
      leap_year = TMT_TRUE;
  }

  return (leap_year);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * FUNCTION   : RollOverFormerLaterDays
 * Function     : Calculation of the number of days before and after
 * Feature Overview   : The number of days from the beginning of the month to the previous day of the specified date,
          and calculates the number of days after a given date, up to the end of the month
 * Input      : TG_TIM_ROLOVR_YMD*  base_ymd  Reference date
 * Output     : u_int32* former_days  Number of days before (number of days before the specified date from the beginning of the month)
          u_int32* later_days Number of days after(Number of days following the specified date until the end of the month)
 * Return value    : None
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static void RollOverFormerLaterDays(TG_TIM_ROLOVR_YMD* base_ymd, u_int32* former_days, u_int32* later_days) {
  int32 leap_year;

  leap_year = ChkLeapYear(base_ymd->year);

  *former_days = base_ymd->day - 1;

  *later_days = kMonth[leap_year][base_ymd->month - 1] - base_ymd->day;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * FUNCTION   : RollOverCalDaysWithinBY
 * Function     : Calculation of the difference in the number of days in the base year
 * Feature Overview   : Calculate the difference in the number of days between the base date and the conversion target date.
           [Restriction]If the base date > conversion target date, the number of days difference is 0.
 * Input     : TG_TIM_ROLOVR_YMD*  base_ymd  Reference date
          TG_TIM_ROLOVR_YMD*  conv_ymd  Conversion Date
 * Output     : None
 * Return value    : u_int32 Difference in the number of days(positive only)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static u_int32 RollOverCalDaysWithinBY(TG_TIM_ROLOVR_YMD* base_ymd, TG_TIM_ROLOVR_YMD* conv_ymd) {
    int32 leap_year; /* Subscripts in the [LEAP YEAR] table */
    u_int32 days; /* Day Difference Calculation Result */
    u_int32 l_days; /* Number of days after the reference month */
    u_int32 f_days; /* Number of days before the conversion target month */
    u_int32 dmy_days; /* Dummy days(For storing unnecessary number of days in this function) */
    u_int32 m_days; /* Sum of the number of days in the month between the base month and the month to be converted */
    int32 cnt; /* A counter that sums the number of days in a month between the base month and the month to be converted */
    u_int16 diff_month; /* Difference between the conversion target month and the base month */
    u_int16 chk_month; /* Number of days of month to be acquired */

    days = 0;

    if (base_ymd->month == conv_ymd->month) {
      if (base_ymd->day <= conv_ymd->day) {
        days = conv_ymd->day - base_ymd->day;
      } else {
        days = 0; /* Set the difference to 0(Negative difference in number of days is not calculated.) */
      }
    } else if (base_ymd->month < conv_ymd->month) {
      RollOverFormerLaterDays(base_ymd, &dmy_days, &l_days);

      m_days = 0;
      diff_month = conv_ymd->month - base_ymd->month;

      leap_year = ChkLeapYear(base_ymd->year);

      /* Calculate the sum of the number of days in the month between the base month and the conversion target month B */
      chk_month = base_ymd->month + 1;
      for (cnt = 0; cnt < (diff_month - 1); cnt++) {
          m_days += kMonth[leap_year][chk_month - 1];
          chk_month++;
      }

      RollOverFormerLaterDays(conv_ymd, &f_days, &dmy_days);

      days = l_days + m_days + f_days + 1; /* Calculate the difference in number of days as A+B+C+1  */

    } else {
        days = 0; /* Set the difference to 0(Negative difference in number of days is not calculated.) */
    }

    return days;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * FUNCTION   : RollOverGetAYearDays
 * Function     : Process of obtaining the number of days per year
 * Feature Overview   : Get the number of days in a given year
 * Input      : u_int16 base_year Year
 * Output     : None
 * Return value    : u_int32 Number of days
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static u_int32 RollOverGetAYearDays(u_int16 base_year) {
  int32 leap_year; /* Subscripts in the [LEAP YEAR] table */
  u_int32 days;

  leap_year = ChkLeapYear(base_year); /* Leap year determination processing */

  if (leap_year == TMT_TRUE) {
    days = TIM_ROLOVR_LEAPYEARDAYS;
  } else {
    days = TIM_ROLOVR_NOTLEAPYEARDAYS;
  }

  return days;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * FUNCTION   : RollOverConvYMDWithoutBY
 * Function     : Date conversion processing outside the base year, month, and day
 * Feature Overview   : Calculate the date shifted by the specified number of days from the specified date.
          *If less than (1024 weeks x 7 days) is specified as the number of days difference, the calculation result is
          returns the specified date(No correction)
 * Input      : TG_TIM_ROLOVR_YMD*  base_ymd  Reference date
          u_int32 days  Difference in the number of days
          u_int32 days_by Number of days after + number of days after month
 * Output     : TG_TIM_ROLOVR_YMD*  conv_ymd  Date of the conversion result
 * Return value    : None
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static void RollOverConvYMDWithoutBY(TG_TIM_ROLOVR_YMD* base_ymd, u_int32 days,
                                                u_int32 days_by, TG_TIM_ROLOVR_YMD* conv_ymd) {
  int32 leap_year; /* Subscripts in the [LEAP YEAR] table */
  u_int32 rest_days;  /* Remaining number of days */
  u_int16 cal_year; /* For year calculation */
  u_int16 cal_month;  /* For monthly calculation */
  u_int32 cal_days; /* To calculate the number of days */

  rest_days = days - days_by; /* Remaining number of days is different from the number of initialization days.-(Number of days after + Number of days after month) */
  cal_year = base_ymd->year + 1; /* The year is set to the year following the year of the initialization reference date. */  /* Ignore -> MISRA-C++:2008 Rule 5-0-5 */

  cal_days = RollOverGetAYearDays(cal_year); /* Process of obtaining the number of days per year */

  while (rest_days > cal_days) {
    rest_days -= cal_days; /* Remaining Days = Remaining Days-Updated as Days of Year */

    cal_year++; /* Increment Year */

    cal_days = RollOverGetAYearDays(cal_year); /* Process of obtaining the number of days per year */
  }

  /* Year Finalization */
  conv_ymd->year = cal_year; /* Year Calculated */

  cal_month = 1; /* Initialize Month January */

  leap_year = ChkLeapYear(conv_ymd->year); /* Leap year determination processing */

  cal_days = kMonth[leap_year][cal_month - 1]; /* Acquisition processing of the number of days/month */ /* Ignore -> MISRA-C++:2008 Rule 5-0-5 */

  while (rest_days > cal_days) {
    rest_days -= cal_days; /* Remaining Days = Remaining Days-Updated as Days of Month */

    cal_month++; /* Increment month */

    cal_days = kMonth[leap_year][cal_month - 1]; /* Acquisition processing of the number of days/month */ /* Ignore -> MISRA-C++:2008 Rule 5-0-5 */
  }

  /* Fix month */
  conv_ymd->month = cal_month; /* Month Calculated */

  /* Date set */
  conv_ymd->day = (u_int16)rest_days; /* Day calculated Remaining number of days */
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * FUNCTION   : RollOverConvYMD
 * Function     : Date conversion processing
 * Feature Overview   : Calculate the date shifted by the specified number of days from the specified date.
          *If the base date is shifted by the number of days but the year is the same as the base date, the result returns the base date
 * Input      : TG_TIM_ROLOVR_YMD*  base_ymd  Reference date
          u_int32 days  Difference in the number of days
 * Output     : TG_TIM_ROLOVR_YMD*  conv_ymd  Date of the conversion result
 * Return value    : None
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static void RollOverConvYMD(TG_TIM_ROLOVR_YMD* base_ymd, u_int32 days, TG_TIM_ROLOVR_YMD* conv_ymd) {
    u_int32 days_by; /* Number of days after + number of days after month(=difference in number of days from the base date to the end of the year) */
    TG_TIM_ROLOVR_YMD tmp_ymd;


    /* (number of days after + number of days after)Set calculation date( 12/31 ) */
    tmp_ymd.year = base_ymd->year;
    tmp_ymd.month = 12;
    tmp_ymd.day = 31;

    days_by = RollOverCalDaysWithinBY(base_ymd, &tmp_ymd); /* Calculation of the difference in the number of days in the base year */

    if (days_by >= days) {
        memcpy(conv_ymd, base_ymd, sizeof(TG_TIM_ROLOVR_YMD)); /* Returns the base date regardless of the number of days difference */

    } else {
        RollOverConvYMDWithoutBY(base_ymd, days, days_by, conv_ymd);
    }
}



/**
 * @brief
 *  Conversion to a time that takes the GPS week correction counter into account
 *
 *  Converting the GPS Week Correction Counter to a Considered Time
 *
 * @param[in]   TG_TIM_ROLOVR_YMD*  base_ymd  Reference date
 * @param[in]   u_int8  gpsweekcorcnt GPS weekly correction counter
 * @param[out]  TG_TIM_ROLOVR_YMD*  conv_ymd  Correction Date
 * @return      none
 * @retval      none
 */
void GPSRollOverConvTime(TG_TIM_ROLOVR_YMD* base_ymd, TG_TIM_ROLOVR_YMD* conv_ymd, u_int8 gpsweekcorcnt) {
    u_int32 days;             /* Difference in the number of days */

    days = TIM_ROLOVR_CALCORCNT_DAYS * gpsweekcorcnt; /* (1024 weeks x 7 days) x GPS week correction counter to calculate the difference in the number of days */

    RollOverConvYMD(base_ymd, days, conv_ymd); /* Date conversion processing */
}
