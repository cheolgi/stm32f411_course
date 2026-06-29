// ============================================================================
//  Lesson 1 — Blink
//  PC13에 연결된 LED를 깜빡인다.
//  핵심 개념: 레지스터 직접 제어(MMIO), RCC 클럭 enable, GPIO 출력
//  ※ 이 보드의 LED는 active-low: 핀이 LOW일 때 켜진다.
// ============================================================================
#include <stdint.h>

// 메모리 맵 레지스터를 "그 주소의 32비트 변수"처럼 다루는 매크로
#define REG(addr) (*(volatile uint32_t*)(addr))

// --- RCC: Reset and Clock Control (주변장치에 클럭을 공급해야 동작한다) ---
#define RCC_AHB1ENR  REG(0x40023830u)          // AHB1 버스 주변장치 클럭 enable

// --- GPIOC (LED1 = PC13) ---
#define GPIOC_BASE   0x40020800u
#define GPIOC_MODER  REG(GPIOC_BASE + 0x00)    // 핀 모드 (2비트/핀: 00입력 01출력 …)
#define GPIOC_BSRR   REG(GPIOC_BASE + 0x18)    // 비트 set/reset (원자적 출력 제어)

#define LED_PIN  13                            // PC13

// 아주 단순한 소프트웨어 지연 (타이머는 Lesson 3에서 배운다)
// HSI 16MHz 기준 대략값 — 정밀하지 않아도 깜빡임 확인엔 충분
static void delay(volatile uint32_t count) {
    while (count--) { __asm__ volatile ("nop"); }
}

int main(void) {
    // 1) GPIOC에 클럭 공급 (AHB1ENR의 bit2 = GPIOC enable)
    RCC_AHB1ENR |= (1u << 2);

    // 2) PC13을 "출력" 모드로 설정
    //    MODER는 핀당 2비트. 먼저 그 2비트를 지우고(00) 01(출력)로 세팅.
    GPIOC_MODER &= ~(3u << (LED_PIN * 2));
    GPIOC_MODER |=  (1u << (LED_PIN * 2));

    // 3) 무한 루프: 켰다 껐다 반복
    for (;;) {
        // BSRR 하위 16비트에 1을 쓰면 해당 핀을 HIGH(set)로,
        // 상위 16비트(+16)에 1을 쓰면 LOW(reset)로 만든다.
        GPIOC_BSRR = (1u << (LED_PIN + 16));   // PC13 = LOW  → LED ON  (active-low!)
        delay(1000000);
        GPIOC_BSRR = (1u << LED_PIN);          // PC13 = HIGH → LED OFF
        delay(1000000);
    }
}
