[ORG 0x00]
[BITS 16] 

SECTION .text

jmp $

times 510 - ( $ - $$ ) db 0x00 
				
db 0x55 
db 0xAA 			
		
		
;ORG 0x00 : 코드의 시작 어드레스를 0x00 으로 설정
;BITS 16  : 이하의 코드는 16 비트 코드로 설정

;SECTION .text : text 섹션(세그먼트) 을 정의

;jmp : jmp 명령 다음에 오는 어드레스로 무조건 이동. goto 와 비슷한 역할
;$ : 현재 라인의 어드레스를 의미

;times : times 명령 다음에 오는 횟수만큼 작업을 반복하라는 의미
;$$ : 현재 어드레스가 포함된 섹션의 시작 어드레스
; $ - $$ : 섹션의 시작을 기준으로 하는 오프셋

;db xx : Defind Byte 의 약자. 현재 어드레스에 1 바이트xx 를 삽입하라는 의미.
;db 0x55, db 0xAA : 부트 로더임ㅇ르 알리는 값 
