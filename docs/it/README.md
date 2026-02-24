
| 🇬🇧 [Inglese](../../README.md) | 🇮🇹 Italiano |
|-----------|----------|

---

# 🧠 NotaMy

![DEMO](../../images/demo.gif)

NotaMy è un gestore gerarchico di appunti tecnici da terminale che riduce il costo mentale del context switching per chi lavora su analisi di lunga durata (pentesting, reverse engineering, wargame).

---

## 🎯 Perché nasce NotaMy

Quando ho iniziato a lavorare su attività di pentesting e analisi di binari, ho capito subito una cosa:

**Il problema non è prendere appunti, ma gestirli nel tempo.**

Un'analisi può durare giorni o mesi. Durante quel tempo accumuli indirizzi di memoria, output di test, disassemblati, comportamenti sospetti, prove fallite, idee da verificare — e tutto questo finisce spesso sparso in file e cartelle.

Il risultato? Confusione. Continuare a cercare informazioni tra file sparsi interrompe il flusso mentale e rallenta l'indagine.

**NotaMy nasce per eliminare questo freno**: 
ridurre il costo del context switching mantenendo tutte le informazioni accessibili da shell, organizzate e facilmente ricercabili.

---

## 🧩 Cos’è NotaMy

NotaMy non è un editor di testo.
Non è un sistema di markdown.
Non è un’applicazione grafica.

È un sistema minimale e terminal-first per organizzare conoscenza tecnica in file di note (contesti) e in strutture ad albero di nodi/tag.

### ⚙️ Caratteristiche principali:

- Gestione di più file di note (contesti) e cambio rapido di contesto
- Struttura gerarchica di nodi con tag, commenti, testo esteso, keywords e riferimenti a file
- Ricerca strutturata (per tag, keyword, contenuto)
- Operazioni rapide completamente dalla tastiera, senza mouse
- Progettato per flussi di lavoro tecnici lunghi e complessi

---

## 🗂️ Il concetto chiave:

Ogni file di note è un progetto o un’indagine. Cambiare file significa cambiare spazio mentale.

Esempio:

``` bash
ntm view file
```

![Output Examples](../../images/View_File.png)

Puoi cambiare contesto con:

```bash
ntm setting 2
```

---

## 🌳 Struttura gerarchica

Le note sono nodi in un albero: ogni nodo può contenere:

* Tag (parola chiave principale)
* Commento (post-it visibile nell'albero)
* Body testuale lungo
* Riferimenti a file
* Keywords per la ricerca

Puoi mettere un post‑it o un intero libro. Tutto nello stesso posto.

Esempio:

```bash
ntm view note
```

![Output Examples"](../../images/View_Note.png)

**#** indica che la nota contiene testo espandibile.

Puoi espandere ogni nodo per vederne il contenuto:

```bash
ntm view note -b
```

![Output Examples](../../images/View_Note_Body.png)

---

##  🔄 Flusso di lavoro (come lo uso)

* Creo o seleziono il file di note (contesto).
* Aggiungo un nodo con tag e commento:

```bash
ntm add note -t "TAG" -c "Breve commento"
```

* Se serve scrivo un testo lungo con l'editor configurato (vim per default):

```bash
ntm add note -t "TAG" -c "Commento" -b
```

si apre l'editor, salvo, esco e il body viene inserito nella nota

* Collego file (exploit, binario, config) direttamente alla nota e uso keywords per trovare tutto velocemente.

---

## 👤 A chi è rivolto

NotaMy è pensato per chi:

* vive nel terminale
* lavora su attività tecniche lunghe e complesse
* gestisce più progetti contemporaneamente
* vuole ordine senza interfacce grafiche
* preferisce la tastiera al mouse

---

## 🚫 A chi NON è rivolto

Probabilmente non fa per te se:

* vuoi sincronizzazione cloud
* vuoi collaborazione multiutente
* vuoi un’interfaccia grafica moderna
* vuoi scrivere documenti in markdown

NotaMy è minimale e focalizzato.

---

## 📝 Storia rapida 

NotaMy nasce da un’esigenza personale:

Ridurre il costo mentale del context switching.

Le note non sono il problema. La complessità lo è.

Ho iniziato nel 2020 con una versione elementare chiamata "Note Management": semplice testo strutturato.
Già allora era utile. Poi sono venuti MemoKeeper e MemoKeeperX, con file separati e tag.
Col tempo l'uso è diventato quotidiano: pentest, studio, idee e piccoli progetti.

NotaMy è la versione matura di quel flusso: minimale, affidabile e adattata al mio modo di lavorare.

> Vuoi sapere perché e come è nato NotaMy? Leggi la [FILOSOFIA](filosofia.md) del progetto.

---

## 🚀 Installazione

```bash
git clone https://github.com/IMprojtech/NotaMy.git
cd NotaMy
make
./bin/ntm
```

### Aggiungere `NotaMy` al PATH

Dopo aver compilato `ntm`, potresti volerlo eseguire da qualsiasi directory senza digitare il percorso completo. Ecco un approccio comune:

### Installazione in una directory già presente nel tuo PATH

1.  Copia o sposta il binario compilato in una posizione standard, ad esempio:

```bash
sudo cp ./bin/ntm /usr/local/bin/
sudo cp ./bin/ntm_nc /usr/local/bin/
```

oppure, se preferisci un'installazione locale per l'utente (senza bisogno di `sudo`):

```bash
mkdir -p "$HOME/.local/bin"
cp ./bin/ntm "$HOME/.local/bin/"
cp ./bin/ntm_nc "$HOME/.local/bin/"
```

Assicurati che questa directory sia nel tuo `PATH`. La maggior parte delle distribuzioni Linux include `/usr/local/bin` per impostazione predefinita. Se hai usato `~/.local/bin`, assicurati che il tuo file di inizializzazione della shell lo aggiunga al `PATH`. Ad esempio, in `~/.bashrc` o `~/.zshrc`:

```bash
# Aggiungi questo se non già presente:
export PATH="$HOME/.local/bin:$PATH"
```

Infine puoi anche semplicemente aggiungere la directory `bin` al tuo file di inizializzazione della shell. Ad esempio, in `~/.bashrc` o `~/.zshrc`:

```bash
export PATH="$HOME/percorso/NotaMy/bin:$PATH"
```

---

## 🐚 Integrazione Zsh opzionale

NotaMy fornisce un wrapper Zsh opzionale che migliora l'esperienza utente senza modificare il programma C principale.

Il wrapper aggiunge:

- Wrapper del comando `ntm` con gestione dell'output migliorata
- Paginazione automatica tramite `bat` 
- Rilevamento TTY corretto
- Funzione `fview` per la visualizzazione interattiva delle note (fzf + bat)
- Funzione `nocolor` per rimuovere le sequenze di escape ANSI
- Cambio temporaneo del file note attivo tramite prefisso numerico

### Cambio temporaneo del file note 

Il wrapper permette di cambiare al volo il file in uso utilizzando il numero del file che si vuole richiamare prima del comando.

```bash
ntm 1 view note   
```

### Installazione 

Aggiungi il file del plugin all'interno del tuo `~/.zshrc`:

```bash
source /percorso/verso/NotaMy/shell/zsh/notamy.plugin.zsh
```

Quindi ricarica la shell:

```bash
source ~/.zshrc
```

> ⚠️ L'integrazione Zsh è completamente opzionale.
> L'applicazione C principale funziona in modo indipendente.

---

## 📘 Documentazione

Un manuale utente completo è disponibile con istruzioni di installazione, esempi di utilizzo e tutti i comandi supportati:

📄 [Leggi il Manuale di NotaMy](manuale.md)

---

## ⚖️ Licenza

Questo programma è distribuito sotto i termini della GNU General Public License (GPL), garantendo la libertà di ridistribuire e modificare il software in conformità con gli standard del software libero.

---

##  ✍️ Autore

Scritto da Catoni Mirko (`IMprojtech`)

---
