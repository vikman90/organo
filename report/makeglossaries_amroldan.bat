set path=%path%;C:\Program Files (x86)\MiKTeX 2.9\miktex\bin


makeindex pfc_amroldan.idx

makeindex -s "pfc_amroldan.ist" -t "pfc_amroldan.alg" -o "pfc_amroldan.acr" "pfc_amroldan.acn"

makeindex -s "pfc_amroldan.ist" -t "pfc_amroldan.glg" -o "pfc_amroldan.sym" "pfc_amroldan.sbl"

makeindex -s "pfc_amroldan.ist" -t "pfc_amroldan.glg" -o "pfc_amroldan.gls" "pfc_amroldan.glo"

bibtex pfc_amroldan.aux
