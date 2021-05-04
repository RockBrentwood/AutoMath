Program=../aut
for i in 0 1 2 3 4 5; do ${Program} -QZ Check$i.aut; done
for i in 0 1 2 3; do ${Program} -QZ Exp$i.aut; done
for i in 0 1; do ${Program} -QZ Foo$i.aut; done
for i in 0 1 2 3; do ${Program} -QZ Item$i.aut; done
for i in 0 1; do ${Program} -QZ Main$i.aut; done
for i in 0 1 2 3 4 5; do ${Program} -QZ Par$i.aut; done
for i in 0 1 2 3; do ${Program} -QZ Value$i.aut; done
