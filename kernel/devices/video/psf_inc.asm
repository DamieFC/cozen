global fb_font

section .data
align 8
fb_font:
	incbin "../kernel/devices/video/terminus.psf"
