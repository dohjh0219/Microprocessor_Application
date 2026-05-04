#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>

#define set_bit(value, bit) ( _SFR_BYTE(value) |= _BV(bit) )
#define clear_bit(value, bit) ( _SFR_BYTE(value) &= ~_BV(bit) )

volatile int count = 0; // 비교일치가 발생한 횟수
uint8_t state = 0;

ISR(TIMER0_COMPA_vect) {
    TCNT0 = 0; // 정상 모드이므로 TCNT0 수동 초기화 
    count++;
    
    // PE4는 OCR0A 주기에 맞춰 소프트웨어적으로 점멸
    if (count == 64) { 
        count = 0;
        state = !state;
        if (state) set_bit(PORTE, 4);
        else clear_bit(PORTE, 4);
    }
}

// 가변저항을 읽기 위한 ADC 초기화 함수
void adc_init() {
    // 기준 전압을 AVCC(5V)로 설정, 채널은 ADC0(PF0) 선택
    ADMUX = (1 << REFS0);
    // ADC 활성화, 분주비 128로 설정 (16MHz / 128 = 125kHz)
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

// ADC 값을 읽어오는 함수 (0 ~ 1023 반환)
uint16_t read_adc() {
    ADCSRA |= (1 << ADSC); // 변환 시작
    while (ADCSRA & (1 << ADSC)); // 변환이 완료될 때까지 대기
    return ADC; // 10비트 결과값 반환
}

int main(void) {
    set_bit(DDRB, 7); // PB7 (OCOA) 출력 설정 
    set_bit(DDRE, 4); // PE4 출력 설정 
    
    clear_bit(PORTB, 7);
    clear_bit(PORTE, 4);

    adc_init(); // ADC 초기화

    TCCR0B |= (1 << CS02) | (1 << CS00); // 분주비 1024 
    TIMSK0 |= (1 << OCIE0A); // 비교일치 인터럽트 A 허용 
    OCR0A = 128; // 초기 비교일치 기준값 

    // PB7(OC0A) 핀은 하드웨어가 자동으로 반전
    TCCR0A |= (1 << COM0A0); 

    sei(); // 전역 인터럽트 허용 

    while (1) {
        // 1. 가변저항 값 읽기 (0 ~ 1023)
        uint16_t pot_value = read_adc(); 
        
        // 2. 10비트(0~1023) 값을 8비트(0~255) 타이머 비교값으로 스케일링
        // 단순히 4로 나누어(우측 시프트 2번) 범위를 맞춥니다.
        uint8_t new_ocr = (pot_value >> 2); 
        
        // 3. 비교값이 너무 작으면 인터럽트가 쉴 새 없이 발생해 시스템이 멈출 수 있으므로 하한선 설정
        if (new_ocr < 10) new_ocr = 10; 

        // 4. 이전 답변에서 강조했던 '타이머 카운터 꼬임' 방지 로직
        cli(); // 레지스터를 건드리는 동안 인터럽트 일시 중지
        OCR0A = new_ocr; // 변수(새로운 비교값) 대입!
        
        // 만약 현재 카운터(TCNT0)가 새로 바꾼 비교값보다 이미 커져 버렸다면?
        // 이번 주기에서 인터럽트가 발생하지 못하고 오작동하므로 강제로 0으로 리셋해줍니다.
        if (TCNT0 >= new_ocr) {
            TCNT0 = 0; 
        }
        sei(); // 인터럽트 재개
    }
    return 0;
}