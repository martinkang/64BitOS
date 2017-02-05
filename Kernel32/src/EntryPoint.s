[ORG 0x00]
[BITS 16]

SECTION .text

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 코드 영역
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
START:
	mov ax, 0x1000
	mov ds, ax
	mov es, ax

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; A20 게이트를 활성화
; BIOS 를 이용한 전환이 실패했을 때 시스템 컨트롤 포트로 전환 시도
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; BIOS 서비스를 사용해서 A20 게이트를 활성화
	mov ax, 0x2401  ; A20 게이트 활성화 서비스 설정
	int 0x15		; BIOS 인터럽트 서비스 호출

	jc.A20GATEERROR	; A20 게이트 활성화가 성공했는지 확인
	jmp .A20GATESUCCESS

.A20GATERROR:
	; 에러 발생 시, 시스템 컨트롤 포트로 전환 시도
	in al, 0x92		; 시스템 컨트롤 포트 0x92 에서 1 바이트를 읽어 AL 레지스터에 저장
	or al, 0x02		; 읽은 값에 A20 게이트 비트 1 을 1 로 설정. 0x10
	and al, 0xFE	; 시스템 리셋 방지를 위해 0xFE 와 AND 연산하여 비트 0 을 0 으로 설정 ( 1111 1110 과 AND 연산)
	out 0x92, al	; 시스템 컨트롤 포트 0x92 에 변경된 값을 1 바이트 설정

.A20GATESUCCESS:
	cli				; 인터럽트가 발생하지 못하도록 설정.
					; 보호모드로 전환하여 인터럽트 관련된 설정이 완료되지 않았으므로.
	lgdt [ GDTR ] 	; GDTR 자료구조를 프로세서에 설정하여 GDT 테이블을 로드


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 보호 모드로 진입
; Disable Paging, Disable Cache, Internal FPU, Disable Align Check
; Enable ProtectedMode
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	mov eax, 0x4000003B 	; PG=0, CD=1, NW=0, AM=0, VIP=0, NE=1, ET=1, TS=1, EM=0, MP=1, PE=1
	mov cr0, eax			; CR0 컨트롤 레지스터에 위에서 저장한 플래그를 설정

	; 커널 코드 세그먼트를 0x00 을 기준으로 하는 것으로 교체하고 EIP 의 값을 0x00 을 기준으로 재생성
	; CS 세그먼트 셀렉터 : EIP
	jmp dword 0x18: ( PROTECTEDMODE - $$ + 0x10000 )

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 보호 모드로 진입
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
[BITS 32]
PROTECTEDMODE:
	mov ax, 0x20	; 보호 모드 커널용 데이터 세그먼트 디스크립터를 AX 레지스터에 저장
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	; 스택을 0x00000000 - 0x0000FFFF 영역에 64KB 크기로 생성
	mov ss, ax
	mov esp, 0xFFFE
	mov ebp, 0xFFFE

	; 화면에 보호 모드로 전환되었다는 메시지를 찍는다.
	push ( SWITCHSUCCESSMESSAGE - $$ + 0x10000 )
	push 2
	push 0
	call PRINTMESSAGE
	add esp, 12

	; C 언어 커널이 조재하는 0x10200 어드레스로 이동하여 C 언어 커널 수행
	jmp dword 0x18: 0x10200 ; CS 세그먼트 셀렉터를 커널 코드 디스크립터 ( 0x08 ) 로 변경하면서 다음 주소로 이동.

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 함수 코드 영역
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
PRINTMESSAGE:
	push ebp
	mov ebp, esp
	push esi
	push edi
	push eax
	push ecx
	push edx

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; X, Y 좌표로 비디오 메모리의 어드레스를 계산함
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; Y 좌표
	mov eax, dword [ ebp + 12 ]
	mov esi, 160
	mul esi
	mov edi, eax

	; X 좌표
	mov eax, dword [ ebp + 8 ]
	mov esi, 2
	mul esi
	add edi, eax

	; 출력할 문자열의 어드레스
	mov esi, dword [ ebp + 16 ]

.MESSAGELOOP:
	mov cl, byte [ esi ]
	cmp cl, 0
	je .MESSAGEEND

	mov byte [ edi + 0xB8000 ], cl

	add esi, 1
	add edi, 2
	jmp .MESSAGELOOP

.MESSAGEEND:
	pop edx
	pop ecx
	pop eax
	pop edi
	pop esi
	pop ebp
	ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 데이터 영역
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 아래의 데이터들을 8 바이트에 맞춰 정렬하기 위해 추가
align 8, db 0

; GDTR 의 끝을 8byte로 정렬하기 위해 추가
dw 0x0000

; GDTR 자료구조 정의
GDTR:
	dw GDTEND - GDT - 1 	  ; 아래에 위치하는 GDT 테이블의 전체 크기
	dd ( GDT - $$ + 0x10000 ) ; 아래에 위치하는 GDT 테이블의 시작 어드레스


; GDT 테이블 정의
GDT:
	; 널 디스크립터, 반드시 0 으로 초기화해야함
	NULLDescriptor:
		dw 0x0000
		dw 0x0000
		db 0x00
		db 0x00
		db 0x00
		db 0x00

	; IA-32e 모드 커널용 코드 세그먼트 디스크립터
	IA_32eCODEDESCRIPTOR:
		dw 0xFFFF	; Limit [15:0]
		dw 0x0000	; Base [15:0]
		db 0x00		; Base [23:16]
		db 0x9A		; P = 1, DPL = 0, Code Segment, Execute/Read
		db 0xAF		; G = 1, D = 0, L = 1, Limit [19:16]
		db 0x00		; Base [31:24]

	; IA-32e 모드 커널용 데이터 세그먼트 디스크립터
	IA_32eDATADESCRIPTOR:
		dw 0xFFFF	; Limit [15:0]
		dw 0x0000	; Base [15:0]
		db 0x00		; Base [23:16]
		db 0x9A		; P = 1, DPL = 0, ㅇㅁㅅㅁ Segment, Read/ㅉ걋ㄷ
		db 0xAF		; G = 1, D = 0, L = 1, Limit [19:16]
		db 0x00		; Base [31:24]

	; 보호 모드 커널용 코드 세그먼트 디스크립터
	CODEDESCRIPTOR:
		dw 0xFFFF	; Limit [15:0]
		dw 0x0000	; Base [15:0]
		db 0x00		; Base [23:16]
		db 0x9A		; P1, DPL=0, Code Segment, Execute/Read
		db 0xCF		; G=1, D=1, L=0, Limit[19:16]
		db 0x00		; Base [31:24]

	; 보호 모드 커널용 데이터 세그먼트 디스크립터
	DATADESCRIPTOR:
		dw 0xFFFF	; Limit [15:0]
		dw 0x0000	; Base [15:0]
		db 0x00		; Base [23:16]
		db 0x92		; P=1, DPL=0,l Data Segment, Read/Write
		db 0xCF		; G=1, D=1, L=0, Limit[19:16]
		db 0x00		; Base [31:24]

GDTEND:

; 보호 모드로 전환되었다는 메시지
SWITCHSUCCESSMESSAGE: db 'Switch To Protected Mode Success', 0

times 512 - ( $ - $$ ) db 0x00
