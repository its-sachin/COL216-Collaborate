main:
	addi $s0, $zero, 0
	addi $s1, $zero, 100
	addi $s3, $zero, 0
	
	lw $t0, 1000($s0)
	addi $s2, $zero, 0
load_loop:	
	addi $s0, $s0, 4
	lw $t2, 1000($s0)
	addi $s3, $s3, 1
	bne $s3, $s1, load_loop
	
exit:
