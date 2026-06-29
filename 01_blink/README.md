# Lesson 1 — Blink (STM32F411 베어메탈)

PC13 LED를 깜빡이는 첫 예제입니다. 이 한 폴더만 VS Code로 열면 **빌드 → 플래시 → 디버깅**이 모두 됩니다.

> 보드의 LED는 **active-low** 입니다 — 핀이 **LOW일 때 켜집니다**.

---

## 0. 필요한 것

| 구분 | 도구 | 확인 명령 |
|---|---|---|
| 컴파일러 | **Arm GNU Toolchain** (`arm-none-eabi-gcc`, `arm-none-eabi-gdb`) | `arm-none-eabi-gcc --version` |
| 플래시/디버그 | **OpenOCD** | `openocd --version` |
| 에디터 | **VS Code** + 확장 2개 (아래) | — |
| 프로그래머 | **RP2040 debugprobe** (CMSIS-DAP) | — |

VS Code 확장 (이 폴더를 열면 "권장 확장 설치" 알림이 뜹니다):
- **C/C++** (`ms-vscode.cpptools`)
- **Cortex-Debug** (`marus25.cortex-debug`)

---

## 1. 개발환경 설치

### 🍎 macOS
```bash
# Homebrew(https://brew.sh)가 설치돼 있다고 가정
brew install --cask gcc-arm-embedded   # arm-none-eabi-gcc + gdb
brew install open-ocd                  # openocd
```

### 🪟 Windows
1. **Arm GNU Toolchain** 설치
   - https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads 에서
     `arm-none-eabi` Windows 설치파일 실행
   - 설치 마지막에 **"Add path to environment variable" 체크** (중요!)
   - (또는 터미널에서) `winget install Arm.GnuArmEmbeddedToolchain`
2. **OpenOCD** 설치
   - `winget install xpack-dev-tools.openocd`  *(또는 scoop: `scoop install openocd`)*
   - 수동: https://github.com/xpack-dev-tools/openocd-xpack/releases 에서 zip 받아
     압축 풀고 `bin` 폴더를 시스템 PATH에 추가

> 설치 후 **새 터미널**을 열어 `arm-none-eabi-gcc --version` 과 `openocd --version` 이
> 동작하는지 꼭 확인하세요. (PATH가 안 잡히면 VS Code도 못 찾습니다.)

---

## 2. 보드 연결 (debugprobe ↔ STM32)

| RP2040 debugprobe | → | STM32F411 보드 |
|---|---|---|
| GP2 | → | **SWCLK** |
| GP3 | → | **SWDIO** |
| GND | → | **GND** |

- debugprobe USB를 PC에 꽂고, **STM32 보드에도 전원**을 공급하세요.
- 🪟 Windows: debugprobe(CMSIS-DAP v2)는 보통 드라이버 없이 바로 인식됩니다.
  혹시 OpenOCD가 `unable to find CMSIS-DAP device` 라고 하면
  [Zadig](https://zadig.akeo.ie)로 해당 인터페이스에 **WinUSB** 드라이버를 설치하세요.

---

## 3. 빌드 · 플래시 · 디버깅 (VS Code)

이 `01_blink` 폴더를 VS Code로 엽니다 (`File ▸ Open Folder`).

| 동작 | 방법 |
|---|---|
| **빌드** | `Ctrl/Cmd + Shift + B` → `blink.elf` 생성 |
| **플래시** | `Terminal ▸ Run Task… ▸ Flash` (빌드도 자동으로 함) |
| **디버깅** | `F5` → `main`에서 멈춤. 중단점 찍고 한 줄씩 실행 가능 |

`** Verified OK **` 가 보이고 보드의 **PC13 LED가 깜빡이면 성공!** 🎉

### 터미널로 직접 하고 싶다면
```bash
# 빌드
arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -Og -g3 -ffreestanding -nostdlib \
  -Wall -Wextra -T stm32f411.ld startup.c main.c -o blink.elf -lgcc

# 플래시
openocd -f openocd.cfg -c "program blink.elf verify reset exit"
```

---

## 4. 폴더 구성

```
01_blink/
├─ main.c            ← 여러분이 고칠 코드 (Blink 본문)
├─ startup.c         ← 리셋 핸들러 + 벡터테이블 (레슨마다 공통)
├─ stm32f411.ld      ← 링커 스크립트 (Flash/RAM 배치)
├─ openocd.cfg       ← debugprobe + STM32F4 연결 설정
└─ .vscode/          ← 빌드/플래시/디버그 버튼 설정
   ├─ tasks.json
   ├─ launch.json
   └─ c_cpp_properties.json
```

---

## 5. 해보기 (과제)
1. 깜빡임 **속도**를 바꿔 보세요 — `delay()` 인자를 키우면 느려집니다.
2. **다른 LED**도 깜빡여 보세요. 이 보드 LED 핀: `PC13, PB12, PB14, PB15, PB1`
   (힌트: PB 핀은 `RCC_AHB1ENR`의 **GPIOB**(bit1)도 켜야 하고, 주소는 `0x40020400`)
3. 두 LED를 **번갈아** 켜 보세요.

> 다음 레슨: **버튼 입력** — 버튼을 누르면 LED가 토글되게 만듭니다.
