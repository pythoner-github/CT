#!/bin/bash
#FPO=res/i18n/09001_russian.po
#FMO=res/i18n/09001_ru.mo
#FPO=res/i18n/09001_de.po
#FMO=res/i18n/09001_de.mo
FPO=res/i18n/09001_lang.po
FMO=res/i18n/09001_lang.mo
#FPO=res/i18n/09001_frence.po
#FMO=res/i18n/09001_fr.mo
#FPO=res/i18n/09001_polish.po
#FMO=res/i18n/09001_polish.mo
#FPO=res/i18n/09001_es.po
#FMO=res/i18n/09001_es.mo

foreach_dir(){
	for file in $1/*
	do
		if [ -d $file ]
		then
			foreach_dir $file
		else
			#echo "$file"
			if [[ $file == *\.cpp || $file == *\.h ]]
			then
				_xgettext $file
			fi
		fi
	done
}

_xgettext(){
	echo "Executing xgettext with $1"
	xgettext --no-location --no-wrap -j --from-code=UTF-8 -k_ -kN_ -o $FPO $1
}
foreach_dir .
