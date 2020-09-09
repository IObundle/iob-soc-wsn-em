\LoadClass[twoside,a4paper]{article}
\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{rep}

\RequirePackage[hidelinks]{hyperref}
\RequirePackage{graphicx}
\RequirePackage{helvet}
\RequirePackage{fancyhdr}
%\RequirePackage{fancyvrb}
%\RequirePackage[includehead,includefoot,top=50pt,bottom=50pt,headheight=24pt,margin=1in]{geometry}
\RequirePackage[includehead,includefoot,top=50pt,bottom=50pt,headheight=24pt]{geometry}
\RequirePackage{hyperref}
\RequirePackage{multirow}
\RequirePackage{verbatim}
\RequirePackage{geometry}
\RequirePackage{color, colortbl}

\definecolor{tab-green}{rgb}{0.3,0.7,0.3}

\geometry{
 a4paper,
 total={170mm,257mm},
 left=20mm,
 top=20mm,
}

\pagestyle{fancy}
\graphicspath{ {./drawings/} {./wavedrom/} }
\setlength{\parskip}{\baselineskip}


\renewcommand{\headrulewidth}{.4pt}
\fancyhead{}
\fancyhead[RO,LE]{
\textbf{Wireless Sensor Networks - Environmental Monitoring}
\\
\textsc{\small{\@category}}
}
\fancyhead[RE,LO]{\includegraphics[scale=.15]{Logo.png}}

\renewcommand{\footruleskip}{10pt}
\renewcommand{\footrulewidth}{.4pt}

\fancyfoot{}
\fancyfoot[LO,RE]{\copyright 2020 INSTITUTO DE TELECOMUNICAÇÕES All rights reserved\hspace{1cm}\url{www.it.pt}\hspace{1cm}}
\fancyfoot[LE,RO]{\thepage}

\newcommand{\category}[1]{\def\@category{#1}}
\renewcommand{\familydefault}{\sfdefault}

\renewcommand{\maketitle}{
\begin{titlepage}
\setlength{\parindent}{0pt}
\setlength{\parskip}{1ex}
\vspace*{100pt}
\Huge{\textbf{\@title}}

\huge{\@category}

\vspace*{2ex}
\includegraphics[keepaspectratio,scale=.7]{Logo.png}

\small{\today}
\vspace*{\fill}
\end{titlepage}
}
