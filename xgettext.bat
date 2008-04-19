..\gettext\bin\xgettext -d fgrun -o po\fgrun.pot --copyright-holder="Frederic Bouvier" -k_ -kN_ src/advanced.cxx src/wizard.cxx src/wizard_funcs.cxx src/advanced_funcs.cxx src/AirportBrowser.cxx src/AirportTable.cxx src/fgfsrc.cxx src/logwin.cxx src/io.cxx src/main.cxx
cd po
..\..\gettext\bin\msgmerge -U fr.po fgrun.pot
..\..\gettext\bin\msgmerge -U nl.po fgrun.pot
..\..\gettext\bin\msgmerge -U pt.po fgrun.pot
..\..\gettext\bin\msgmerge -U de.po fgrun.pot
..\..\gettext\bin\msgmerge -U it.po fgrun.pot
..\..\gettext\bin\msgmerge -U pl.po fgrun.pot
cd ..