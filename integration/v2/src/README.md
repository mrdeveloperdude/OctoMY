# V2 - the cleanup

This is the v2 folder. It contains the src and test of OctoMY project rebuilt from the ground up.

The original folders contain some bugs and architectual problems that are very difficult to get rid of in a traditional development cycle.

This readme describes the process we wish to use to get rid of those problems, with some additional perks added since this provide such an excellent opportunity for them


## Procss


1. Always have a working system - Never introduce changes so large that the system becomes uncompilable or unrunnable for more than a single development cycle (a few days).

2. Incorporate parts one at the time - Each subsystem should be copied and made to work one at the time.

3. Perform full QA per part - Each subsystem should receive a full QA during the transition to v2 including the changes listed below. Each part shall have a comment written about it that judges it's quality, noting the potential of improvement according to the norm below.

4. Improve each single part in steps until the grading reaches satisfactory MVP score (norms tagged with [MVP]).

5. Once all parts are transitioned to v2, the full system should be tested thoroughly to see that it works.

6. v2 done - old src and test can now be deleted with good conscience to be replaced by the new mast breed.

7. Further refinement - Continue improving parts to increase their score in the grading system.


## Norm for quality grading

The norm for quality grading follows the points below.
Each point will have a subjective grading with a short comment of the most important aspects considered as lacking during the grading.

A. Builds - Build should result in valid build artifact
   PASS / FAIL
   [MVP]

B. Builds cleanly (no warnings) - All warnings should be ironed out of the code
   PASS / FAIL

C. Part runs without errors - It should run without problems
   PASS / FAIL
   [MVP]

D. Part runs without errors cross platform - It should run without problems on all platforms
   PASS / FAIL

E. Remove redundant code - Each part shall only have it's necessary code copied over.
   PASS / FAIL

F. Restructure code - Each part shall be restructured to follow the norm of v2 (see separate section).
   PASS / FAIL

G. Add full test coverage - Each part should have full test coverage.
   0-100 = percentage of code that is covered by tests

H. Add full documentation - Each part should have sufficient documentation in comments and in documentation site according to norm of documentation (see separate section).
   0-100 = percentage of code that follows norm for documentation

I. Make independend - Each part should build on it's own with minimal dependencies necessary.
   X = code is unecessarily dependent 
   V = code is independent

J. Optimize build time - Each part should have it's build time measured and optimized.
   B = milliseconds of build time

K. Build cross platform - Each part should be tested for it's ability to build on different platforms.
   P = number of platforms it builds for

L. Performance - Each part shall have a performance test associated with it that allows performance to be measured.
   


## Norm for restructuring

A. librest renamed to libSOMETHINGELSE

B. libutil renamed to libSOMETHINGELSE

A. widgets and ui should be split up and merged into their correct places ( see separate section)

A. resources should be split up and  merged into their correct places ( see separate section)

C. libcore split up and parts merged into their correct places ( see separate section)

D. utility.cpp split up (See separate section)

E. Modernized logging facilities

F. Introduce namespace hierarchy? (See separate section)

G. Licensing - each part should have details about it's licensing documented so that there is no question of what licensing applies


## Norm for documentation

A. All class / struct named descriptively

B. All class / struct introduced with short comment in declaration

C. All function / method named descriptively

D. All function / method introduced with short comment in declaration

E. All function / method parameters named descriptively in declaration

F. All function / method parameters named descriptively in definition

G. Complex parts should have a page in the qwiki explaining how it is supposed to work and how it is supposed to be integrated

## Norm for build restructuring

## Splitting of libcore

## Splitting of utility.cpp

## Namespace hierarchy


