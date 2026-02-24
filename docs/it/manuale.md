
---

# 📒 Documentazione `NotaMy`

NotaMy (ntm) è uno strumento da riga di comando per la gestione gerarchica di conoscenza tecnica e contesti di lavoro.

Organizza le informazioni in file di note (contesti) strutturati come alberi di nodi.
Ogni nodo può contenere:

* un tag identificativo
* un commento breve
* un body testuale esteso
* keywords per la ricerca
* riferimenti a file
* un hash univoco

Il sistema consente l’inserimento, la modifica, la visualizzazione e la ricerca strutturata delle informazioni direttamente dalla shell.

> NotaMy è progettato per supportare flussi di lavoro tecnici di lunga durata, mantenendo separati i contesti e riducendo la dispersione delle informazioni.

---

## 📌 SINOSSI

```bash
ntm [COMMAND] [SUBCOMMAND] [OPTIONS]  # Comando completo con sotto-comando
ntm [COMMAND] [OPTIONS]               # Comando semplice (es: find, help)
```

---

## 🧭 COMANDI DISPONIBILI

| Comando    | SottoComando | Descrizione                                                            |
| ---------- | ------------ | ---------------------------------------------------------------------- |
| `add`      | `note`       | Inserisce una nuova nota                                               |
|            | `file`       | Aggiunge un nuovo file di archivio delle note                          |
| `view`     | `note`       | Visualizza le note presenti                                            |
|            | `file`       | Visualizza l’elenco dei file note disponibili                          |
|            | `tag`        | Mostra la gerarchia dei tag                                            |
| `find`     |              | Cerca una o più note tramite criteri (tag, hash, keywords, data, ecc.) |
| `modify`   |              | Modifica una nota esistente                                            |
| `organize` |              | Sposta una nota all’interno della gerarchia                            |
| `remove`   |              | Elimina una nota                                                       |
| `setting`  |              | Imposta un file di note come attivo                                       |
| `editor`   |              | Imposta l’editor di testo (`vim`, `nano`, `nul`)                       |
| `backup`   |              | Crea un backup del file note attivo                                    |
| `help`     |              | Mostra l’help generale                                                 |

---

## ⚙️ OPZIONI DISPONIBILI

> Le opzioni variano a seconda del comando, quelle comuni sono riportate qui di seguito:

### `add note`

| Opzione breve | Opzione lunga  | Descrizione                                         |
| ------------- | -------------- | --------------------------------------------------- |
| `-t`          | `--tag`        | Inserisce un tag (obbligatorio, default: `"?"`)     |
| `-c`          | `--comment`    | Aggiunge un commento alla nota (obbligatorio)       |
| `-k`          | `--keywords`   | Inserisce una o più parole chiave separate da spazi |
| `-f`          | `--file`       | Allega un file                                      |
| `-b`          | `--body`       | Apre l’editor per scrivere il corpo della nota      |
| `-i`          | `--input`      | Disattiva l’editor, consente redirezione da stdin   |
| `-p`          | `--protection` | Abilita la cifratura della nota                     |

### `add file`

| Argomenti obbligatori | Opzioni         |
| --------------------- | --------------- |
| `<percorso/file>`     | `-c` (commento) |

### `view note`, `view tag`, `view file`

| Opzione breve | Opzione lunga  | Descrizione                         |
| ------------- | -------------- | ----------------------------------- |
| `-e`          | `--extended`   | Visualizzazione estesa (hash, data) |
| `-b`          | `--body`       | Mostra anche il corpo delle note    |
| `-p`          | `--protection` | Visualizza anche le note protette   |

### `find`

| Opzione          | Descrizione                                             |
| ---------------- | ------------------------------------------------------- |
| `-t`             | Cerca per tag                                           |
| `-h`             | Cerca per hash specifico                                |
| `-k`             | Cerca per keywords                                      |
| `-d`             | Cerca per data (relativa o assoluta)                    |
| `-f`             | Apre il file allegato con l’editor impostato            |
| `-o`             | Stampa il contenuto del file allegato invece di aprirlo |
| `-e`, `-b`, `-p` | Come in `view`                                          |

### `modify`

| Argomento obbligatorio | Opzioni come in `add note` |
| ---------------------- | -------------------------- |
| `<hash>`               |                            |

### `organize`

| Argomento obbligatorio | Destinazione                                  |
| ---------------------- | --------------------------------------------- |
| `<hash nota>`          | `"."` (radice), `up`, `down`, o un altro hash |

### `remove`

| Argomento obbligatorio | Descrizione            |
| ---------------------- | ---------------------- |
| `-t`, `-h`             | Elimina per tag o hash |

---

## ✍️ INSERIMENTO DELLE NOTE

### Esempio base

```bash
ntm add note -t "tag1" -c "commento" -k "key1 key2" -f "/percorso/file.txt" -b
```

> L’editor selezionato verrà avviato per scrivere il corpo della nota.

### Allegare un file

* Usa `-f` seguita dal percorso completo o abbreviato tramite `$HOME` o `$(pwd)`:

```bash
ntm add note -t "docs" -c "relazione" -f "$HOME/relazioni/report.txt"
```

### Inserire keywords

* Usa `-k` seguita da una o più parole chiave separate da uno spazio.

```bash
ntm add note -t "progetto" -c "descrizione" -k "java API spring"
```

> Le keywords migliorano la ricerca futura e sono visibili nella modalità `view note -be`.

### Usare variabili o comandi dinamici

Sfrutta variabili d'ambiente e comandi per inserire dinamicamente le informazioni.

```bash
ntm add note -t "log" -c "registrato il $(date)"
```

### Redirezione dell'output

Puoi redirigere l'output di un comando direttamente nel corpo della nota.

```bash
ls | ntm add note -t "directory" -c "contenuto attuale" -bi
```

> `-i` è necessario per leggere da stdin.

### Annidamento diretto delle note

Usa i Caratteri Speciali  `<`, `>` in tag per annidare le note

```bash
ntm add note -t "progetti>API" -c "Nota dentro API"
```

oppure

```bash
ntm add note -t "API<progetti" -c "Nota dentro API"
```

> Entrambi i formati sono validi.

### Protezione delle note

* Usa `-p` per proteggere la nota.

```bash
ntm add note -t "sicurezza" -c "contenuto cifrato" -p
```

---

## 🔎 VISUALIZZAZIONE E RICERCA

* Per offrire un'esperienza d'uso davvero efficiente, NotaMy punta su una visualizzazione intuitiva e una ricerca potente.

### Panoramica dei Tag

Per ottenere una panoramica della gerarchia dei tag, usa il comando `view tag`:

```bash
ntm view tag
ntm view tag -e   # Mostra anche gli hash
```

### Elenco delle Note

Puoi usare il comando `view note` per ottenere una lista di tutte le note presenti.

```bash
ntm view note
```

### Ricerca per tag, hash, keywords o data

```bash
ntm find -t "progetti"
ntm find -h "a1b2"
ntm find -k "API spring"
```

> Personalizza la visualizzazione con le opzioni aggiuntive come: `-e`, `-b`, `-p`.

### Ricerca temporale

* **Relativa** (usa `Y`, `M`, `D`, `h`, `m`, `s`) per gli intervalli di tempo:

```bash
ntm find -d 2D    # Ultimi 2 giorni
```

* **Assoluta** per date complete o intervalli di date:

```bash
ntm find -d "2024-05"       # Tutto maggio 2024
ntm find -d "2024-01 to 2024-03"
```

### Espansione e riduzione della gerarchia

```bash
ntm find -t "tag1+"    # Estende la visuale a figli e sottofigli
ntm find -t "tag1-"    # Mostra solo fratelli
```

> La ricerca è **case insensitive** e **parziale** (basta l’inizio, se univoco).

---

## 📂 GESTIONE FILE ALLEGATI

### Aprire o stampare file allegati

```bash
ntm find -ft "tag1"   # Apre file con l’editor
ntm find -fot "tag1"  # Stampa il file nel terminale
```

> `-o` è necessario per scrivere in stdout.

---

## ✏️ MODIFICA E GESTIONE STRUTTURA

### Modificare una nota

```bash
ntm modify "5b43" -t "nuovo_tag" -c "nuovo commento"
ntm modify "5b43" -b   # Modifica del corpo
```

>  Se l’editor è `nul`, sovrascrive il testo; con `vim/nano`, modifica in-place.

### Riorganizzare i nodi

```bash
ntm organize "5b43" "ff46"   # Sposta 5b43 dentro ff46
ntm organize "5b43" .        # Sposta alla radice
ntm organize "5b43" up       # Sposta tra i fratelli
```

### Eliminare una nota

```bash
ntm remove -h "5b43"
```

⚠️ **Attenzione:** La rimozione è ricorsiva: verranno cancellati anche i discendenti!

---

## ⚙️ IMPOSTAZIONI

### Imposta editor

Puoi scegliere tra: `nul`, `vim`, `nano`.

```bash
ntm editor vim   # Scelte: vim, nano, nul
```

### Aggiungi file di archivio

Aggiungi un nuovo file con il comando `add file`:

```bash
ntm add file "$HOME/percorso/nuovo_note.X" -c "Note del 2024"
```

### Visualizza file di archivio disponibili

```bash
ntm view file
```

### Imposta file attivo

```bash
ntm setting 0
```

> L'indice è visibile con `view file`

---

## 🛡️ BACKUP

```bash
ntm backup
```

Crea una copia di sicurezza del file attivo.

---

## ❓ HELP

```bash
ntm help
```

---

## 🐚 Shell Zsh

NotaMy include un wrapper opzionale per Zsh che aggiunge funzionalità utili senza modificare il programma principale.

Il wrapper migliora la gestione dell’output e introduce comandi che semplificano il lavoro su file di note e testi lunghi.

---

### Gestione automatica dell’output con `bat`

Alcuni comandi, ad esempio:

```bash
ntm view note -b
```

possono produrre output molto lunghi, rendendo difficile la lettura diretta nella shell.

Normalmente sarebbe necessario usare:

```bash
ntm view note -b | bat
```

Il wrapper esegue automaticamente questo redirect verso `bat` quando opportuno, migliorando:

* leggibilità
* paginazione
* evidenziazione sintattica
* navigazione

Se invece si desidera forzare la stampa diretta su stdout (senza `bat`), è sufficiente utilizzare `cat`:

```bash
ntm view note -b | cat
```

---

### Cambio temporaneo di contesto

Il wrapper consente di richiamare un file di note diverso senza modificare il contesto attivo in modo permanente.

Scenario standard: supponiamo di trovarci nel file 5.

```bash
ntm setting 1
ntm view note
ntm setting 5
```

Con il wrapper, le stesse operazioni possono essere eseguite in un solo comando:

```bash
ntm 1 view note
```

In questo modo il file `1` viene utilizzato solo per quell’esecuzione, mantenendo invariato il contesto corrente.

---

### Ricerca interattiva con `fview` (fzf + bat)

La funzione `fview` permette di effettuare una ricerca interattiva all’interno delle body delle note utilizzando `fzf`.

Esempio base:

```bash
ntm view note -b
```

In presenza di testi lunghi, la ricerca manuale può risultare scomoda.
Con `fview` è possibile eseguire:

```bash
ntm view note -b | fview
```

Viene avviata una sessione `fzf`:

1. Digitare la parola o frase da cercare
2. Selezionare la riga desiderata
3. Premere `Invio`

L’output verrà riaperto nella visualizzazione standard tramite `bat`, posizionato direttamente sulla riga selezionata.

---

> ⚠️ Il wrapper Zsh è completamente opzionale.
> NotaMy funziona correttamente anche senza di esso.

---

## ✅ NOTE FINALI

- Tag e hash non sono case-sensitive.
- Sono consentite abbreviazioni purché non ambigue.
- Eseguendo `ntm_nc` il programma viene avviato in modalità senza colori (no ANSI escape sequences).

---

## 📁 CONFIGURAZIONE

> Il comportamento di NotaMy può essere personalizzato tramite il file di configurazione:

```
~/.config/ntm.conf
```

All’interno puoi configurare:

* Colori dell’interfaccia testuale
* Editor predefinito
* Percorsi dei file di default

---
