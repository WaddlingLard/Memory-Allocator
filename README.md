# Project: Memory Allocator

* Author: Brian Wu
* Class: CS452 Section #002
* Semester: Spring 2025

## Overview

An implementation of the buddy system in c using a balloc module.

## Reflection

Wow. I'm happy its complete. It's all I ever wanted from this homework assignment.

It's rewarding to understand it conceptually and equally as rewarding to have the code. I want to say that my knowledge of pointers has increased at least 3 fold after doing the HW1 refresher. Also, it opened my eyes that implementations can have so many avenues, but the way I took was one that is convoluted and honestly overthought. BUT, I can imagine that the code can slowly be refactored down into a nice package. 

The biggest things was understanding how to use a bitmap and how freelist should be structured. The balloc module acted more as a front and doesn't really provide much to the functionality of the allocator other than being the shiny headpiece. Going into detail about the bitmaps, it took me a second before it clicked that a bit in the bitmap represented two buddies. That was a revelation when I figured that out because thinking about offsets, transitioning between blocksize levels in the freelist, and worrying of walking onto a segfault mine washed away. It trivialized the thought process and made me more confident in the design process and allowed me to focus on how to code the implementation rather than what should the implementation even look like. 

This assignment was no joke and I'll remember this one made me get little sleep on the days coming up to the due date. I felt like I was making decent progress in the first and second week, but man I was not aware at how much more there was to do.

Takeaway note, thank god memory allocators already exist.

## Compiling and Using

To compile the code, first cd into the `hw2` directory.
Then, run this command 

```make```

and then...

```./memoryalloc```

OR... if you want to run immediately after building use

```make run```

Just make sure the file structure is not disturbed and all should be good!

Note: Main will contain all the tests. They are mostly just output statements but there will be enough information to verify the allocator is working. There is also a little deq test at the bottom to show they are linking up and working properly using the strcmp() method.

## Results 

Balloc looks really solid. I'm happy with how it turned out and it does a great job. It probably is not even close to being the most optimal solution, but I'm so happy I figured it out and got all of the methods working. I would give a confidence rating of 90-95% that it works but more testing would always be nice to confirm anyone's suspicions.

The deq looks promising, I haven't been able to implement anything that crazy so you'd definitely need to take a better look at it. However, it does look ok for the time being so with at least 70% confidence it appears to work! I know that isn't the most convincing argument but it does the most basic iterations of the operations of put, get, ith, and rem so that is a start.

## Sources used

None
----------

## Notes

* This README template is using Markdown. Here is some help on using Markdown: 
[markdown cheatsheet](https://github.com/adam-p/markdown-here/wiki/Markdown-Cheatsheet)


* Markdown can be edited and viewed natively in most IDEs such as Eclipse and VS Code. Just toggle
between the Markdown source and preview tabs.

* To preview your README.md output online, you can copy your file contents to a Markdown editor/previewer
such as [https://stackedit.io/editor](https://stackedit.io/editor).
