Output assets/stderr.gif

Set Width 860
Set Height 610
Set Shell "bash"
Set Theme "Catppuccin Mocha"
Set FontFamily "Menlo"
Set FontSize 16
Set Padding 24
Set WindowBar Colorful
Set BorderRadius 10
Set Margin 20
Set MarginFill "#181825"
Set TypingSpeed 40ms

Hide
Type `d=$(mktemp -d) && cp leak_hunter.h assets/demo/main.c $d && cd $d && export PS1="\[\e[38;5;117m\]$ \[\e[0m\]" && clear`
Enter
Show

Type "cc -DLEAK_HUNTER_STDERR main.c -o app && ./app"
Sleep 300ms
Enter
Sleep 2s
Type "ls"
Sleep 300ms
Enter
Sleep 4s
