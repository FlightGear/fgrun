..\gettext\bin\xgettext -d fgrun -o po\fgrun.pot --copyright-holder="Frederic Bouvier" -k_ -kN_ -D src advanced.cxx wizard.cxx wizard_funcs.cxx advanced_funcs.cxx AirportBrowser.cxx AirportTable.cxx fgfsrc.cxx logwin.cxx io.cxx main.cxx
cd po
..\..\gettext\bin\msgmerge -U fr.po fgrun.pot
..\..\gettext\bin\msgmerge -U nl.po fgrun.pot
cd ..