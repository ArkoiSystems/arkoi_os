.section .init
	# GCC will put the contents of crtend.o's .init section here
	popl %ebp
	ret

.section .fini
	# GCC will put the contents of crtend.o's .fini section here
	popl %ebp
	ret