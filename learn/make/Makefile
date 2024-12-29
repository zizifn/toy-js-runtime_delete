# A rule - this rule will only run if file0.txt doesn't exist.
# file0.txt:
# 	echo "foo" > file0.txt
# 	# Even comments in these 'recipe' sections get passed to the shell.
# 	# Try `make file0.txt` or simply `make` - first rule is the default.

# file1.txt:
# 	echo "foo" > file1.txt
# 	# Even comments in these 'recipe' sections get passed to the shell.
# 	# Try `make file0.txt` or simply `make` - first rule is the default.

maker: *.txt
	@echo "foo"
	@echo $^

ex0.txt ex1.txt: maker

# convert svg into png
%.png: %.svg
	@echo "Converting $< to $@"
	inkscape --export-filename=$@ $^

name=ted
name2="sarah"
name4+=$(name) $(name2)
name5 := $(name) $(name2)
name="joe"
sourcefiles = $(wildcard *.c */*.c)

echo:
	@echo $(sourcefiles)
	@echo $(name2)
	@echo $(name3)
	@echo $(name4)
	@echo $(name5)



sport = tennis
# foo = true
ifdef foo
bar = hello
endif

# Conditional compilation
report:
	@echo $(bar)
ifeq ($(sport),tennis)
	@echo 'game, set, match'
else
	@echo "They think it's all over; it is now"
endif



