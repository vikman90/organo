set path=%path%;C:\Program Files (x86)\MiKTeX 2.9\miktex\bin


makeindex pfc_vmfdez.idx

makeindex -s "pfc_vmfdez.ist" -t "pfc_vmfdez.alg" -o "pfc_vmfdez.acr" "pfc_vmfdez.acn"

makeindex -s "pfc_vmfdez.ist" -t "pfc_vmfdez.glg" -o "pfc_vmfdez.sym" "pfc_vmfdez.sbl"

makeindex -s "pfc_vmfdez.ist" -t "pfc_vmfdez.glg" -o "pfc_vmfdez.gls" "pfc_vmfdez.glo"

