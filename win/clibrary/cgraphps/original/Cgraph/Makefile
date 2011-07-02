all:
	for dir in source ; do cd $$dir ; make ; cd .. ; done
	for dir in source ; do cd $$dir ; make install ; cd .. ; done
	for dir in examples ; do cd $$dir ; make ; cd .. ; done

clean:
	for dir in source examples ; do cd $$dir ; make clean; cd .. ; done

