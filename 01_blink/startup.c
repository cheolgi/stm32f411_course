// ============================================================================
//  startup.c — STM32F411 최소 스타트업 코드 + 벡터테이블 (Cortex-M4)
//  리셋되면 CPU가 가장 먼저 실행하는 코드. .data/.bss 초기화 후 main() 호출.
//  (이 파일은 모든 레슨에서 거의 그대로 재사용한다)
// ============================================================================
#include <stdint.h>

// 링커 스크립트(stm32f411.ld)가 정의해 주는 심볼들
extern uint32_t _estack;   // 스택 꼭대기 주소
extern uint32_t _sdata, _edata, _sidata;   // .data 영역 (RAM 시작/끝, Flash 원본)
extern uint32_t _sbss, _ebss;              // .bss 영역 (RAM 시작/끝)
extern int main(void);

// 리셋 핸들러: 전원/리셋 직후 진입점
void Reset_Handler(void) {
    // 1) 초기값 있는 전역변수(.data)를 Flash에서 RAM으로 복사
    uint32_t *src = &_sidata, *dst = &_sdata;
    while (dst < &_edata) *dst++ = *src++;
    // 2) 0으로 초기화되는 전역변수(.bss)를 0으로
    for (dst = &_sbss; dst < &_ebss;) *dst++ = 0;
    // 3) 사용자 코드 시작
    main();
    for (;;);   // main이 반환하면 멈춤
}

// 처리하지 않는 예외/인터럽트는 여기서 멈춘다 (디버깅 시 여기 걸리면 원인 추적)
void Default_Handler(void) { for (;;); }

// 벡터테이블: Flash 맨 앞(.isr_vector)에 위치해야 한다.
//  [0]=초기 스택 포인터, [1]=리셋 핸들러, 그 뒤로 예외 핸들러들.
__attribute__((section(".isr_vector"), used))
void (*const g_vectors[])(void) = {
    (void (*)(void)) &_estack,   // 0  초기 SP
    Reset_Handler,               // 1  Reset
    Default_Handler,             // 2  NMI
    Default_Handler,             // 3  HardFault
    // 이 레슨에서는 나머지 인터럽트를 쓰지 않으므로 생략(필요해지면 확장)
};
