% IZU Projekt 2 - Uloha 22
% Vypracoval: Michal Pysik (login: xpysik00)

uloha22([],[],[]). % Kdyz jsou oba seznamy vyprazdnene, zbytek jejich ZIPu musi byt take prazdny
uloha22(As, [], As). % kdyz je druhy seznam uz vyprazdneny, jejich ZIP musi pokracovat zbytkem prvniho seznamu
uloha22([], Bs, Bs). % to stejne plati symetricky pro druhy seznam

% Tvrdime ze tyto dva vyrazy maji ekvivalentni logickou hodnotu
uloha22([A|As],[B|Bs],[A,B|Cs]) :- uloha22(As,Bs,Cs).

% vzdy zkontroluje ze vrcholovy prvek ZIP seznamu se rovna vrcholu 1. seznamu, a prvek hned pod nim vrcholu 2. seznamu, vsech 4 prvku se pak zbavi
% Pokud takto dojde do jednoho ze stavu popsanych pravdivymi vyroky nahore, musi byt cely vyrok take pravdivy
