
-----

# 🧠 NotaMy

Gestore di note basato su terminale con tag gerarchici e collegamento file

-----

## 📖 Descrizione del Programma

NotaMy è un'applicazione da riga di comando per Linux progettata per semplificare la gestione e l'organizzazione di note e informazioni importanti. Offre un'interfaccia intuitiva per inserire, categorizzare e visualizzare note, consentendo agli utenti di creare una struttura gerarchica utilizzando tag personalizzati.

-----

## ✨ Caratteristiche Principali

  - **Creazione di Note:**
    Gli utenti possono creare note con testo, commenti e percorsi di file, offrendo una panoramica completa e pertinente.

  - **Organizzazione Gerarchica:**
    Utilizzando i tag, il programma permette di organizzare le note in strutture ad albero, consentendo una categorizzazione flessibile e personalizzata.

  - **Collegamento a File:**
    Le note possono essere associate a file specifici, creando collegamenti diretti per un accesso rapido ed efficiente alle risorse correlate.

  - **Facilità d'uso da Terminale:**
    Il programma è progettato per l'interazione da riga di comando, offrendo un'esperienza veloce e senza fronzoli.

-----

## 📸 Esempi di Output

![Output Examples](../../images/Screenshot_ntm.png)

*Esempio di ntm che mostra una struttura di note gerarchica e i dettagli di un tag selezionato (Git\_Basics).*

-----

## 💡 Applicazioni Pratiche

  - **Penetration Testing:**
    NotaMy è uno strumento essenziale durante la fase di raccolta informazioni del pentesting, permettendo agli operatori di archiviare efficacemente le scoperte e accedervi rapidamente quando necessario.

  - **Sviluppo Software:**
    Gli sviluppatori possono utilizzare il programma per annotare dettagli di implementazione, correzioni di bug e collegare note specifiche a parti del codice.

  - **Gestione Progetti:**
    Nella gestione dei progetti, il programma può essere utilizzato per registrare dettagli del progetto, traguardi e assegnare tag a compiti specifici.

  - **Ricerca Accademica:**
    Studenti o ricercatori possono organizzare informazioni di ricerca, annotazioni e idee in modo strutturato, facilitando la navigazione rapida e il recupero dei dati.

  - **Risorse Educative:**
    In contesti educativi, il programma può essere utilizzato per organizzare materiale didattico, lezioni e collegamenti a risorse esterne.

  - **Amministrazione di Sistema:**
    Per gli amministratori di sistema, può essere uno strumento prezioso per registrare configurazioni di rete, script importanti e dettagli di manutenzione.

  - **Creatività e Scrittura:**
    Scrittori e creativi possono trovarlo utile per annotare idee, frammenti di testo e dettagli della trama in modo organizzato.

  - **Gestore di Password:**
    Con la modalità protetta, può fungere anche da gestore di password leggero.

La versatilità del programma consente agli utenti di adattarlo a una vasta gamma di contesti in cui è fondamentale una gestione strutturata dei dati, facilitando la rapida consultazione delle informazioni precedentemente salvate.

-----

## ⬇️ Download e Compilazione

```sh
    git clone https://github.com/IMprojtech/NotaMy.git
    cd NotaMy
    make
    ./bin/ntm
```

-----

## 🔗 Aggiungere `NotaMy` al PATH

Dopo aver compilato `ntm`, potresti volerlo eseguire da qualsiasi directory senza digitare il percorso completo. Ecco un approccio comune:

### Installazione in una directory già presente nel tuo PATH

1.  Copia o sposta il binario compilato in una posizione standard, ad esempio:

```sh
    sudo cp ./bin/ntm /usr/local/bin/
    sudo cp ./bin/ntm_nc /usr/local/bin/
```

oppure, se preferisci un'installazione locale per l'utente (senza bisogno di `sudo`):

```sh
    mkdir -p "$HOME/.local/bin"
    cp ./bin/ntm "$HOME/.local/bin/"
    cp ./bin/ntm_nc "$HOME/.local/bin/"
```

Assicurati che questa directory sia nel tuo `PATH`. La maggior parte delle distribuzioni Linux include `/usr/local/bin` per impostazione predefinita. Se hai usato `~/.local/bin`, assicurati che il tuo file di inizializzazione della shell lo aggiunga al `PATH`. Ad esempio, in `~/.bashrc` o `~/.zshrc`:

```sh
    # Aggiungi questo se non già presente:
    export PATH="$HOME/.local/bin:$PATH"
```

Infine puoi anche semplicemente aggiungere la directory `bin` al tuo file di inizializzazione della shell. Ad esempio, in `~/.bashrc` o `~/.zshrc`:

```sh
    export PATH="$HOME/percorso/NotaMy/bin:$PATH"
```
-----

## 📘 Documentazione

Un manuale utente completo è disponibile con istruzioni di installazione, esempi di utilizzo e tutti i comandi supportati:

📄 [Leggi il Manuale di NotaMy](manuale.md)

-----

## ⚖️ Licenza

Questo programma è distribuito sotto i termini della GNU General Public License (GPL), garantendo la libertà di ridistribuire e modificare il software in conformità con gli standard del software libero.

-----

##  ✍️ Autore

Scritto da Catoni Mirko (`IMprojtech`)

-----

## 🌍 Lingue Disponibili

- 🇬🇧 [Inglese](../../README.md)
- 🇮🇹 Italiano (questa pagina)
