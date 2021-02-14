# Programming Languages Homework 1

An EBNF-to-BNF Bison rule translator built using Flex and Bison. Actions
are completely ignored during translation.

## 1. Project structure

```
project
├─.vscode
│  └─settings.json                  VSCode workspace settings
│
├─dist                          Build artifacts (created after build)
│  ├─bin
│  │  └─trans                           Executable binary of translator
│  │
│  └─c_src                          Folder to hold generated C codes
│     ├─trans.tab.c                     Parser C code generated by Bison
│     ├─trans.tab.h                     Parser C header generated by Bison
│     └─trans.yy.c                      Tokenizer C code generated by Flex
│
├─examples                      Bison-with-EBNF Examples used to test the translator
│  └─...
│
├─src                           Translater source folder
│  ├─trans.l                        Tokenizer source in Flex
│  ├─trans.y                        Parser source in Bison
│  ├─treenode.cpp                   AST tree node source in C++
│  └─treenode.h                     treenode header file
│
├─load_modules.sh               Shell script for fast loading Flex and Bison
├─make.sh                       Shell script for easily building the translator
├─run_example.sh                Shell script for easily running examples
│
...
```

## 2. Build

To build the translator, simply run the following shell script via CLI:

```sh
./make.sh
```

Then, the executable binary should be available at `dist/bin/trans`

## 3. Usage

### 3.1 `dist/bin/trans`

When running, the translator binary reads from standard input (`stdin`) and
output its translation to standard output (`stdout`).

#### 3.1.1 Direct input and output (not recommended)

You can type all the content that needs to be translated and end it with
`Ctrl+D`. But as is shown by the following example, the output would be
mixed up with the input, which is messy.

```
$ ./dist/bin/trans
%%
a: digit+;
%%

%%

a:
    a_quant
  ;

a_quant:
    digit
  |
    digit a_quant
  ;

%%
```

#### 3.1.2 Input file content through pipe (recommended)

```
$ cat examples/cal1.y | ./dist/bin/trans
```

#### 3.1.3 Redirect output to file (recommended)

```
$ cat example/cal1.y | ./dist/bin/trans > example/cal1.out.y
```

## 3.2 Example runner / helper script: `run.sh`

This project includes 7 example bison source files, 2 with BNF rules
(`cal1.y`, `cal2.y`), the others with EBNF rules (`test1.y`, `test2.y`,
`test3.y`, `test4.y`, `test5.y`).

To translate an example file or any file, you can try:

```
$ ./run.sh ./examples/test1.y
```

`./examples/test1.y` could be replaced with any path to a valid Bison file
with BNF/EBNF rules.

## 4. Limitations

### 4.1 Actions are ignored

Because new productions could be generated during translation, and it's
difficult to match original actions to new productions, actions are
completely ignored during translation, and the translation result would
contain no action code blocks.

### 4.2 Nested blocks in actions are not supported

Because it's impossible to represent the recursive structure of nested
blocks using a single standard regular expression, action block with only
one level of matching curly brackets can be matched by the regular
expression. A possible solution would be further defining C/C++ tokens and
grammar structure in Flex and Bison. But that would be out of topic for
this project.

For example, the following file would throw an error when being translated.

```bison
%%

digit:
    '2' {
      if (flag) {
        flag = 0;
        $$ = new node($1);
      } // if block cannot be matched
    }
  |
    '3'
  ;

%%
```

### 4.3 Generated rules are not optimal

The generated rules are correct BNF rules, but sometimes can be too verbose
and even with meaningless nodes.

For example, a possible translation result could be as follow:

```
%%
digit:
    digit_expr
  ;

digit_expr:
    'a'
  |
    'b'
  ;
%%
```

In this example, non-terminal `digit_expr` is completely unnecessary as
`digit_expr` here is just an alias for `digit`.

This is due to how the Bison rules are defined in this project to support
EBNF . Because `digit_expr` could also be used inside, for example, a
repetition rule like `number -> digit_expr+` where `digit_expr` is a
necessary non-terminal for it to be translated into BNF rule
`number -> digit_expr | digit_expr number`.

Optimizations can be done to eliminate redundancies like this. For example,
by visiting the AST, find out rules like `A -> B` where `B` is only used
here. Then just delete this rule and rename `B` as `A`. Unfortunately due
to time limit, this has not been implemented yet.
