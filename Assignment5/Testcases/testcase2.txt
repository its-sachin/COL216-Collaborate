main:
	addi $s0, $zero, 5
	addi $s1, $zero, 0
	addi $s2, $zero, 1
	addi $s3, $zero, 1
	addi $s4, $zero, 1

check: 
	slt $t0, $s0, $s2
	beq $t0, $zero, run
	bne $t0, $zero, loopexit
	
run:
	add $s1, $s1, $s0
	mul $s3, $s3, $s0
	sub $s0, $s0, $s4
	j check
	
loopexit:
	add $s1, $s1, $zero
	mul $s3, $s3, $s4

exit:		
