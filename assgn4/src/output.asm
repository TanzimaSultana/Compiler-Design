

# ------------------------------------------------------------------

	# Declare main as a global function
	# The label 'main' represents the starting point
	.text
	.globl main
main:

	# Exit the program by means of a syscall.
	# The code for exit is 10. 
	li $v0, 10 	# Sets $v0 to 10 to select exit syscall
	syscall 	# Exit



# ------------------------------------------------------------------

	# All memory structures are placed after the .data assembler directive
	.data

a:	.word 9


