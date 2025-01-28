# Sequent Proof GUI

A GUI application for proving sequents using modal (LTL) or linear logic and generating LaTeX-based proofs. This application supports simple linear logic sequents and LTL sequents with $\Box$ and $\circ$ operators.
Allowed rules are $\land, \lor, \neg$ since you can express any formula with those rules. 

---

## Features
- **User-Friendly GUI**:
  - Input sequents in the Latex format. E.g. \Box p \vdash \Box (p \lor q)
  - Specify custom page width and height for LaTeX-generated proofs. This is useful if prooftree gets too big.
  - Toggle between "LTL" and "LTL1" rules using radio buttons. LTL1 is a new sequent calculi for LTL and works **ONLY** in a unary fragment of LTL. LTL1 completely eliminates cyclic type axiom and proof trees are much smaller. Use LTL for normal linear logic and classic LTL with loop axioms.
- **Proof Generation**:
  - Generates LaTeX proofs using `bussproofs` package.
  - Supports modal logic rules such as $\Box$ and $\circ$. Since eventually operator can be expressed with negation and box rule.
- **PDF Viewer**:
  - Embedded PDF viewer to preview the generated proofs.
  - Automatic LaTeX compilation using `pdflatex`.

---

## Installation

Clone the repository
```bash
git clone git@github.com:maksimiak/SeqProver.git
```

### Using executable application

Navigate to release folder \build\Desktop_Qt_6_8_1_MinGW_64_bit-Release and run **SeqProver.exe**.

### Prerequisites (if you want to extend the project)
- **Qt Framework**: Ensure you have the Qt 6 libraries installed, including `Qt6Pdf` and `Qt6Widgets`.
- **LaTeX**: Install a LaTeX distribution, such as [MiKTeX](https://miktex.org/) or [TeX Live](https://www.tug.org/texlive/).
- **C++ Compiler**: A modern C++ compiler like MSVC, GCC, or Clang.

## Examples

### Fill example sequent

Fill the text box with a predefined sequent. For custom sequents provide them in tex syntax.

![image](https://github.com/user-attachments/assets/c521a6ee-8bca-4b97-b66f-99336774bd7e)

### Prove

Generates proof.tex with bussproofs tree. You can copy the tree inside your .tex file.

![image](https://github.com/user-attachments/assets/5828c441-4444-41f5-94cb-a14251363fe5)

### Compile and show PDF

Compiles .tex file and displays it in PDF viewer

![image](https://github.com/user-attachments/assets/b46e0662-dc8a-49ba-a34d-14ad9bc93a3c)




