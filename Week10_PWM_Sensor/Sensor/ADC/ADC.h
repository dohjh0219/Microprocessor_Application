#ifndef ADC_LIBRARY_
#define ADC_LIBRARY_

#include <avr/io.h>

#define SINGLE_CONVERSION 0
#define FREE_RUNNING      1

void ADC_init(unsigned char channel, int mode);
int read_ADC(void);

#endif /* ADC_H_ */