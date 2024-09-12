	.ORIG x3000  
    LEA R0, TEN		;This instruction will be loaded into memory location x3000
	LDW R1, R0, #0
    ADD R1, R1, x-5

    LSHF R1, R1, #1
    RSHF R1, R1, #1

    TRAP x3c
    HALT

    TEN	.FILL x0100
    .END