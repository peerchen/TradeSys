
#include <time.h>


static inline unsigned long long time_convert(int year, int month, int day, int hour, int min, int sec, int micro_sec)
{
	struct tm timein;
	timein.tm_year = year > 1980 ? (year - 1900) : year;
	timein.tm_mon = month-1;
	timein.tm_mday = day-1;
	timein.tm_hour = hour-1;
	timein.tm_min = min;
	timein.tm_sec = sec;
	time_t tm = mktime(&timein);

	//time_t tm = time(NULL);

	return (unsigned long long)tm*1000+micro_sec;
}