main:
	addi $s0, $zero, 1000
	addi $s1, $zero, 2500
	addi $t0, $zero, 1
	addi $t1, $zero, 2
	addi $t2, $zero, 3
	addi $t3, $zero, 4

	sw $t0, 0($s0)	#store 1 at location 1000
	sw $t1, 0($s1)	#store 2 at location 2500
	
	sw $t2, 4($s0)	#store 3 at location 1004
	sw $t4, 4($s1)	#store 4 at location 2504
	
	lw $t5, 0($s0)
	lw $t6, 0($s1)
	lw $t7, 4($s0)
	lw $t8, 4($s1)


exit:
