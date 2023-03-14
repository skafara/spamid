# spamid
Multiplatform Spam Identifier Using Naive Bayes Classifier

Seminar Work of KIV/PC - "Programming in C"

> University of West Bohemia, Pilsen

## Compilation

Compile with `make` or `cmake` using provided Makefiles or CMakeLists.txt.

## Usage

`spamid <spam> <spam-cnt> <ham> <ham-cnt> <test> <test-cnt> <out-file>`

	<spam>     - Training spam files pattern.
	<spam-cnt> - Training spam files count.
	<ham>      - Training ham files pattern.
	<ham-cnt>  - Training ham files count.
	<test>     - Tested files pattern.
	<test-cnt> - Tested files count.
	<out-file> - Output file name.

## Example

`spamid spam 1234 ham 1234 test 12 result.txt`

	Classifier learns 1234 spam files ("spam1.txt" ... "spam1234.txt").
	Classifier learns 1234 ham files ("ham1.txt" ... "ham1234.txt").
	Classifier classifies 12 tested files ("test1.txt" ... "test12.txt").
	Output is printed to file "result.txt".
