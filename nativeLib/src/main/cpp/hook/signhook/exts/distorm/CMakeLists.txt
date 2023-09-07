include_directories(.)

add_library(distorm STATIC
        decoder.c
        distorm.c
        instructions.c
        insts.c
        mnemonics.c
        operands.c
        prefix.c
        textdefs.c
        wstring.c)
