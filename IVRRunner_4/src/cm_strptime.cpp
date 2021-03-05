#ifdef WIN32
#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <time.h>

#include "cm_strptime.h"

#define match_char(ch1, ch2) if (ch1 != ch2) return NULL

# define match_string(cs1, s2) \
  (strncmp ((cs1), (s2), strlen (cs1)) ? 0 : ((s2) += strlen (cs1), 1)) 

# define get_alt_number(from, to, n) \
	/* We don't have the alternate representation.  */			      \
	get_number(from, to, n)
#define recursive(new_fmt) \
	(*(new_fmt) != '\0'							      \
	&& (rp = strptime_internal (rp, (new_fmt), tm, decided, era_cnt)) != NULL)


#define get_number(from, to, n) \
	do {									      \
	int __n = n;							      \
	val = 0;								      \
	while (*rp == ' ')							  \
	++rp;								          \
	if (*rp < '0' || *rp > '9')					  \
	return NULL;							      \
	do {								          \
	val *= 10;							          \
	val += *rp++ - '0';						      \
	} while (--__n > 0 && val * 10 <= to && *rp >= '0' && *rp <= '9');	      \
	if (val < from || val > to)					  \
	return NULL;							      \
	} while (0)

static char const weekday_name[][10] =
{
	"Sunday", "Monday", "Tuesday", "Wednesday",
		"Thursday", "Friday", "Saturday"
};
static char const ab_weekday_name[][4] =
{
	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};
static char const month_name[][10] =
{
	"January", "February", "March", "April", "May", "June",
		"July", "August", "September", "October", "November", "December"
};
static char const ab_month_name[][4] =
{
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
# define HERE_D_T_FMT "%a %b %e %H:%M:%S %Y"
# define HERE_D_FMT "%m/%d/%y"
# define HERE_AM_STR "AM"
# define HERE_PM_STR "PM"
# define HERE_T_FMT_AMPM "%I:%M:%S %p"
# define HERE_T_FMT "%H:%M:%S"

const unsigned short int __mon_yday[2][13] =
{
	/* Normal years.  */
	{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
		/* Leap years.  */
	{ 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};

/* Status of lookup: do we use the locale data or the raw data?  */
enum locale_status { not, loc, raw };

#ifndef __isleap
/* Nonzero if YEAR is a leap year (every 4 years,
except every 100th isn't, and every 400th is).  */
# define __isleap(year)	\
	((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0))
#endif


static struct tm * localtime_r (time_t *t, struct tm *tp)
{
	struct tm *l = localtime (t);
	if (! l)
		return 0;
	*tp = *l;
	return tp;
}

/* Compute the day of the week.  */
static void day_of_the_week (struct tm *tm)
{
	/* We know that January 1st 1970 was a Thursday (= 4).  Compute the
	the difference between this data in the one on TM and so determine
	the weekday.  */
	int corr_year = 1900 + tm->tm_year - (tm->tm_mon < 2);
	int wday = (-473
		+ (365 * (tm->tm_year - 70))
		+ (corr_year / 4)
		- ((corr_year / 4) / 25) + ((corr_year / 4) % 25 < 0)
		+ (((corr_year / 4) / 25) / 4)
		+ __mon_yday[0][tm->tm_mon]
		+ tm->tm_mday - 1);
		tm->tm_wday = ((wday % 7) + 7) % 7;
}

/* Compute the day of the year.  */
static void day_of_the_year (struct tm *tm)
{
	tm->tm_yday = (__mon_yday[__isleap (1900 + tm->tm_year)][tm->tm_mon]
	+ (tm->tm_mday - 1));
}
static char * strptime_internal (const char *rp, const char *fmt, struct tm *tm, enum locale_status *decided, int era_cnt)
{
	const char *rp_backup;
	int cnt;
	size_t val;
	int have_I, is_pm;
	int century, want_century;
	int want_era;
	int have_wday, want_xday;
	int have_yday;
	int have_mon, have_mday;
	struct era_entry *era;
	have_I = is_pm = 0;
	century = -1;
	want_century = 0;
	want_era = 0;
	era = NULL;

	have_wday = want_xday = have_yday = have_mon = have_mday = 0;
	while (*fmt != '\0')
	{
		/* A white space in the format string matches 0 more or white
		space in the input string.  */
		if (isspace (*fmt))
		{
			while (isspace (*rp))
				++rp;
			++fmt;
			continue;
		}
		/* Any character but `%' must be matched by the same character
		in the iput string.  */
		if (*fmt != '%')
		{
			match_char (*fmt++, *rp++);
			continue;
		}

		++fmt;
		/* We need this for handling the `E' modifier.  */
		start_over: 
		/* Make back up of current processing pointer.  */
		rp_backup = rp;
		switch (*fmt++)
		{
		case '%':
			/* Match the `%' character itself.  */
			match_char ('%', *rp++);
			break;
		case 'a':
		case 'A':
			/* Match day of week.  */
			for (cnt = 0; cnt < 7; ++cnt)
			{
				if(
					(*decided != loc) && 
					( match_string (weekday_name[cnt], rp) || match_string (ab_weekday_name[cnt], rp) )
				)
				{
					*decided = raw;
					break;
				}
			}
			if (cnt == 7)
				/* Does not match a weekday name.  */
				return NULL;
			tm->tm_wday = cnt;
			have_wday = 1;
			break;
		case 'b':
		case 'B':
		case 'h':
			/* Match month name.  */
			for (cnt = 0; cnt < 12; ++cnt)
			{
				if (match_string (month_name[cnt], rp)
					|| match_string (ab_month_name[cnt], rp))
				{
					*decided = raw;
					break;
				}
			}
			if (cnt == 12)
				/* Does not match a month name.  */
				return NULL;
			tm->tm_mon = cnt;
			want_xday = 1;
			break;
		case 'c':
			/* Match locale's date and time format.  */

			if (!recursive (HERE_D_T_FMT))
				return NULL;
			want_xday = 1;
			break;
		case 'C':
			/* Match century number.  */
			get_number (0, 99, 2);
			century = (int)val;
			want_xday = 1;
			break;
		case 'd':
		case 'e':
			/* Match day of month.  */
			get_number (1, 31, 2);
			tm->tm_mday = (int)val;
			have_mday = 1;
			want_xday = 1;
			break;
		case 'F':
			if (!recursive ("%Y-%m-%d"))
				return NULL;
			want_xday = 1;
			break;
		case 'x':
			/* Fall through.  */
		case 'D':
			/* Match standard day format.  */
			if (!recursive (HERE_D_FMT))
				return NULL;
			want_xday = 1;
			break;
		case 'k':
		case 'H':
			/* Match hour in 24-hour clock.  */
			get_number (0, 23, 2);
			tm->tm_hour = (int)val;
			have_I = 0;
			break;
		case 'I':
			/* Match hour in 12-hour clock.  */
			get_number (1, 12, 2);
			tm->tm_hour = (int)val % 12;
			have_I = 1;
			break;
		case 'j':
			/* Match day number of year.  */
			get_number (1, 366, 3);
			tm->tm_yday = (int)val - 1;
			have_yday = 1;
			break;
		case 'm':
			/* Match number of month.  */
			get_number (1, 12, 2);
			tm->tm_mon = (int)val - 1;
			have_mon = 1;
			want_xday = 1;
			break;
		case 'M':
			/* Match minute.  */
			get_number (0, 59, 2);
			tm->tm_min = (int)val;
			break;
		case 'n':
		case 't':
			/* Match any white space.  */
			while (isspace (*rp))
				++rp;
			break;
		case 'p':
			/* Match locale's equivalent of AM/PM.  */
			if (!match_string (HERE_AM_STR, rp))
				if (match_string (HERE_PM_STR, rp))
					is_pm = 1;
				else
					return NULL;
			break;
		case 'r':
			if (!recursive (HERE_T_FMT_AMPM))
				return NULL;
			break;
		case 'R':
			if (!recursive ("%H:%M"))
				return NULL;
			break;
		case 's':
			{
				/* The number of seconds may be very high so we cannot use
				the `get_number' macro.  Instead read the number
				character for character and construct the result while
				doing this.  */
				time_t secs = 0;
				if (*rp < '0' || *rp > '9')
					/* We need at least one digit.  */
					return NULL;

				do
				{
					secs *= 10;
					secs += *rp++ - '0';
				}
				while (*rp >= '0' && *rp <= '9');

				if (localtime_r (&secs, tm) == NULL)
					/* Error in function.  */
					return NULL;
			}
			break;
		case 'S':
			get_number (0, 61, 2);
			tm->tm_sec = (int)val;
			break;
		case 'X':
			/* Fall through.  */
		case 'T':
			if (!recursive (HERE_T_FMT))
				return NULL;
			break;
		case 'u':
			get_number (1, 7, 1);
			tm->tm_wday = (int)val % 7;
			have_wday = 1;
			break;
		case 'g':
			get_number (0, 99, 2);
			/* XXX This cannot determine any field in TM.  */
			break;
		case 'G':
			if (*rp < '0' || *rp > '9')
				return NULL;
			/* XXX Ignore the number since we would need some more
			information to compute a real date.  */
			do
			++rp;
			while (*rp >= '0' && *rp <= '9');
			break;
		case 'U':
		case 'V':
		case 'W':
			get_number (0, 53, 2);
			/* XXX This cannot determine any field in TM without some
			information.  */
			break;
		case 'w':
			/* Match number of weekday.  */
			get_number (0, 6, 1);
			tm->tm_wday = (int)val;
			have_wday = 1;
			break;
		case 'y':
			/* Match year within century.  */
			get_number (0, 99, 2);
			/* The "Year 2000: The Millennium Rollover" paper suggests that
			values in the range 69-99 refer to the twentieth century.  */
			tm->tm_year = (int)val >= 69 ? (int)val : (int)val + 100;
			/* Indicate that we want to use the century, if specified.  */
			want_century = 1;
			want_xday = 1;
			break;
		case 'Y':
			/* Match year including century number.  */
			get_number (0, 9999, 4);
			tm->tm_year = (int)val - 1900;
			want_century = 0;
			want_xday = 1;
			break;
		case 'Z':
			/* XXX How to handle this?  */
			break;
		case 'E':
			/* We have no information about the era format.  Just use
			the normal format.  */
			if (*fmt != 'c' && *fmt != 'C' && *fmt != 'y' && *fmt != 'Y'
				&& *fmt != 'x' && *fmt != 'X')
				/* This is an illegal format.  */
				return NULL;

			goto start_over;
		case 'O':
			switch (*fmt++)
			{
			case 'd':
			case 'e':
				/* Match day of month using alternate numeric symbols.  */
				get_alt_number (1, 31, 2);
				tm->tm_mday = (int)val;
				have_mday = 1;
				want_xday = 1;
				break;
			case 'H':
				/* Match hour in 24-hour clock using alternate numeric
				symbols.  */
				get_alt_number (0, 23, 2);
				tm->tm_hour = (int)val;
				have_I = 0;
				break;
			case 'I':
				/* Match hour in 12-hour clock using alternate numeric
				symbols.  */
				get_alt_number (1, 12, 2);
				tm->tm_hour = (int)val - 1;
				have_I = 1;
				break;
			case 'm':
				/* Match month using alternate numeric symbols.  */
				get_alt_number (1, 12, 2);
				tm->tm_mon = (int)val - 1;
				have_mon = 1;
				want_xday = 1;
				break;
			case 'M':
				/* Match minutes using alternate numeric symbols.  */
				get_alt_number (0, 59, 2);
				tm->tm_min = (int)val;
				break;
			case 'S':
				/* Match seconds using alternate numeric symbols.  */
				get_alt_number (0, 61, 2);
				tm->tm_sec = (int)val;
				break;
			case 'U':
			case 'V':
			case 'W':
				get_alt_number (0, 53, 2);
				/* XXX This cannot determine any field in TM without
				further information.  */
				break;
			case 'w':
				/* Match number of weekday using alternate numeric symbols.  */
				get_alt_number (0, 6, 1);
				tm->tm_wday = (int)val;
				have_wday = 1;
				break;
			case 'y':
				/* Match year within century using alternate numeric symbols.  */
				get_alt_number (0, 99, 2);
				tm->tm_year = (int)val >= 69 ? (int)val : (int)val + 100;
				want_xday = 1;
				break;
			default:
				return NULL;
			}
			break;
		default:
			return NULL;
		}
	}

	if (have_I && is_pm)
		tm->tm_hour += 12;

	if (century != -1)
	{
		if (want_century)
			tm->tm_year = tm->tm_year % 100 + (century - 19) * 100;
		else
			/* Only the century, but not the year.  Strange, but so be it.  */
			tm->tm_year = (century - 19) * 100;
	}
	if (want_era)
		return NULL;

	if (want_xday && !have_wday)
	{
		if ( !(have_mon && have_mday) && have_yday)
		{
			/* We don't have tm_mon and/or tm_mday, compute them.  */
			int t_mon = 0;
			while (__mon_yday[__isleap(1900 + tm->tm_year)][t_mon] <= tm->tm_yday)
				t_mon++;
			if (!have_mon)
				tm->tm_mon = t_mon - 1;
			if (!have_mday)
				tm->tm_mday =
				(tm->tm_yday
				- __mon_yday[__isleap(1900 + tm->tm_year)][t_mon - 1] + 1);
		}
		day_of_the_week (tm);
	}
	if (want_xday && !have_yday)
		day_of_the_year (tm);
	return (char *) rp;
}

char * strptime (const char *buf, const char *format, struct tm *tm)
{
	enum locale_status decided;
	decided = raw;
	return strptime_internal (buf, format, tm, &decided, -1);
}

#endif // WIN32

