#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define NMAXMOVIMENTI 100
#define NMAXUTENTI 100

struct movimento
{
    char tipologia; // un solo carattere - E=Entrata, U=Uscita
    float importo;
    int data;              // numero ∈ [1,366] per rappresentare il giorno dell’anno
    char motivazione[100]; // non contiene spazi
};

struct utente
{
    char nome[100];
    char cognome[100];
    char codice_fiscale[17];
    int eta;
    char citta_residenza[100];
    char provincia_residenza[100];
    int nrMovimenti;
    struct movimento movimenti_effettuati[NMAXMOVIMENTI];
};
void convertiInMaiuscolo(char *p)
{
    while (*p)
    {
        *p = toupper(*p);
        p++;
    }
}
int caricaMovimenti(char percorsoFile[], char codice_fiscale[], struct movimento movimenti[])
{
    FILE *file;
    int num_movimenti = 0;
    char cf[17];
    struct movimento movimento_corrente;

    // apri il file in modalità lettura
    file = fopen(percorsoFile, "r");

    // controlla se il file è stato aperto correttamente
    if (file == NULL)
    {
        printf("Impossibile aprire il file!");
        return -1;
    }

    // leggi i movimenti dal file
    while (fscanf(file, "%s %c %f %d %100s", cf, &movimento_corrente.tipologia, &movimento_corrente.importo,
                  &movimento_corrente.data, movimento_corrente.motivazione) == 5)
    {
        // verifica se il CF corrisponde a quello cercato
        if (strcmp(cf, codice_fiscale) == 0)
        {
            // aggiungi il movimento al vettore
            movimenti[num_movimenti] = movimento_corrente;
            num_movimenti++;

            // verifica se abbiamo raggiunto il numero massimo di movimenti
            if (num_movimenti >= NMAXMOVIMENTI)
            {
                break;
            }
        }
    }

    // chiudi il file
    fclose(file);

    // restituisci il numero di movimenti caricati
    return num_movimenti;
}

int caricaUtenti(char percorsoFile[], struct utente elenco[])
{
    FILE *file;
    file = fopen(percorsoFile, "r");

    if (file == NULL)
    {
        printf("Errore nell'apertura del file.\n");
        return -1;
    }

    int nrUtenti = 0;
    char buffer[200];

    while (fgets(buffer, 200, file))
    {
        // Leggi i dati dell'utente dal file
        char nome[100], cognome[100], codice_fiscale[17], citta_residenza[100], provincia_residenza[100];
        int eta;

        sscanf(buffer, "%s %s %s %s %s %d", nome, cognome, codice_fiscale, citta_residenza, provincia_residenza, &eta);

        // Crea un nuovo utente
        struct utente nuovoUtente;
        strcpy(nuovoUtente.nome, nome);
        strcpy(nuovoUtente.cognome, cognome);
        strcpy(nuovoUtente.codice_fiscale, codice_fiscale);
        nuovoUtente.eta = eta;
        strcpy(nuovoUtente.citta_residenza, citta_residenza);
        strcpy(nuovoUtente.provincia_residenza, provincia_residenza);

        // Leggi i movimenti dell'utente dal file movimenti.txt
        char percorsoFileMovimenti[100] = "movimenti.txt";
        int nrMovimenti = 0;
        nrMovimenti = caricaMovimenti(percorsoFileMovimenti, nuovoUtente.codice_fiscale,
                                      nuovoUtente.movimenti_effettuati);

        nuovoUtente.nrMovimenti = nrMovimenti;
        elenco[nrUtenti] = nuovoUtente;
        nrUtenti++;

        if (nrUtenti == NMAXUTENTI)
        {
            printf("Attenzione: il numero massimo di utenti è stato raggiunto.\n");
            break;
        }
    }

    fclose(file);
    return nrUtenti;
}

void stampaUtente(struct utente *ptrUtente)
{
    printf("Nome: %s\n", ptrUtente->nome);
    printf("Cognome: %s\n", ptrUtente->cognome);
    printf("Codice fiscale: %s\n", ptrUtente->codice_fiscale);
    printf("Età: %d\n", ptrUtente->eta);
    printf("Città di residenza: %s\n", ptrUtente->citta_residenza);
    printf("Provincia di residenza: %s\n", ptrUtente->provincia_residenza);
    printf("Movimenti:\n");
    for (int j = 0; j < ptrUtente->nrMovimenti; j++)
    {
        printf("%d) Tipologia: %c, Importo: %.2f, Data: %d, Motivazione: %s\n",
               j + 1, ptrUtente->movimenti_effettuati[j].tipologia,
               ptrUtente->movimenti_effettuati[j].importo,
               ptrUtente->movimenti_effettuati[j].data,
               ptrUtente->movimenti_effettuati[j].motivazione);
    }
}

void stampaElencoUtenti(struct utente utenti[])
{
    for (int i = 0; i < caricaUtenti("utenti.txt", utenti); i++)
    {
        stampaUtente(utenti);
    }
}

float saldoMovimenti(struct movimento movimenti[], int nmMovimentiUtente)
{
    float saldo = 0.0;
    for (int i = 0; i < nmMovimentiUtente; i++)
    {
        if (movimenti[i].tipologia == 'E')
        {
            saldo += movimenti[i].importo;
        }
        else if (movimenti[i].tipologia == 'U')
        {
            saldo -= movimenti[i].importo;
        }
    }
    return saldo;
}

void stampaSaldoUtenti(struct utente utenti[])
{
    for (int i = 0; i < NMAXUTENTI; i++)
    {
        printf("Utente %d - %s %s:\n", i + 1, utenti[i].nome, utenti[i].cognome);
        float saldo = saldoMovimenti(utenti[i].movimenti_effettuati, utenti[i].nrMovimenti);
        printf("Saldo: %.2f\n", saldo);
    }
}

struct utente *cercaUtente(char codice_fiscale[], struct utente elenco[])
{
    for (int i = 0; i < NMAXUTENTI; i++)
    {
        if (strcmp(elenco[i].codice_fiscale, codice_fiscale) == 0)
        {
            return &elenco[i];
        }
    }
    return NULL; // se non trova l'utente restituisce NULL
}

float calcolaMedia(float *valori, int numValori)
{
    float somma = 0;
    for (int i = 0; i < numValori; i++)
    {
        somma += valori[i];
    }
    return somma / (float)numValori;
}

float calcolaMinimo(float *valori, int numValori)
{
    float minimo = 0;
    for (int i = 0; i < numValori; i++)
    {
        if (valori[i] < minimo)
        {
            minimo = valori[i];
        }
    }
    return minimo;
}

float calcolaMassimo(float *valori, int numValori)
{
    float massimo = 0;
    for (int i = 0; i < numValori; i++)
    {
        if (valori[i] > massimo)
        {
            massimo = valori[i];
        }
    }
    return massimo;
}

void statisticheUtente(char CF[], struct utente elenco[], int giornoMin, int giornoMax)
{
    struct utente *ptrUtente = cercaUtente(CF, elenco);
    if (ptrUtente == NULL)
    {
        printf("Utente non trovato.\n");
        return;
    }

    int numMovimenti = ptrUtente->nrMovimenti;
    float entrate[numMovimenti];
    float uscite[numMovimenti];
    int countEntrate = 0;
    int countUscite = 0;

    for (int i = 0; i < numMovimenti; i++)
    {
        if (giornoMin <= ptrUtente->movimenti_effettuati[i].data &&
            ptrUtente->movimenti_effettuati[i].data <= giornoMax)
        {
            if (ptrUtente->movimenti_effettuati[i].tipologia == 'E')
            {
                entrate[countEntrate] = ptrUtente->movimenti_effettuati[i].importo;
                countEntrate++;
            }
            else if (ptrUtente->movimenti_effettuati[i].tipologia == 'U')
            {
                uscite[countUscite] = ptrUtente->movimenti_effettuati[i].importo;
                countUscite++;
            }
        }
    }

    printf("Statistica personali per %s %s\n", ptrUtente->nome, ptrUtente->cognome);
    printf("Periodo: %d - %d\n", giornoMin, giornoMax);
    printf("Numero di entrate: %d\n", countEntrate);
    printf("Numero di uscite: %d\n", countUscite);
    if (countEntrate > 0)
    {
        float mediaEntrate = calcolaMedia(entrate, countEntrate);
        float minimoEntrate = calcolaMinimo(entrate, countEntrate);
        float massimoEntrate = calcolaMassimo(entrate, countEntrate);
        printf("Media entrate: %.2f\n", mediaEntrate);
        printf("Valore minimo entrate: %.2f\n", minimoEntrate);
        printf("Valore massimo entrate: %.2f\n", massimoEntrate);
    }
    if (countUscite > 0)
    {
        float mediaUscite = calcolaMedia(uscite, countUscite);
        float minimoUscite = calcolaMinimo(uscite, countUscite);
        float massimoUscite = calcolaMassimo(uscite, countUscite);
        printf("Media entrate: %.2f\n", mediaUscite);
        printf("Valore minimo entrate: %.2f\n", minimoUscite);
        printf("Valore massimo entrate: %.2f\n", massimoUscite);
    }
}
char cf[17];

void scriviMovimentoSuFile(char percorso[], struct movimento nuovoMovimento)
{
    FILE *fp;
    fp = fopen(percorso, "a");
    if (fp == NULL)
    {
        printf("Errore nell'apertura del file!\n");
    }
    else
    {
        fprintf(fp, "%s %c %d %.2f %s\n", cf, nuovoMovimento.tipologia, nuovoMovimento.data, nuovoMovimento.importo, nuovoMovimento.motivazione);
    }
    fclose(fp);
}

struct movimento aggiungiMovimento(struct utente *ptrUtente)
{
    if (ptrUtente == NULL)
    {
        printf("Errore\n");
        return;
    }

    struct movimento nuovoMovimento;
    char tipoMovimento;
    float importo;
    int data;
    char motivazione[100];
    printf("Inserisci tipo movimento (E=Entrata, U=Uscita): ");
    scanf(" %c", &tipoMovimento);
    convertiInMaiuscolo(&tipoMovimento);
    printf("Inserisci importo: ");
    scanf("%f", &importo);

    printf("Inserisci la data: ");
    scanf("%d", &data);

    printf("Inserisci la motivazione: ");
    scanf("%s", motivazione);
    convertiInMaiuscolo(motivazione);

    // setta il tipo del movimento
    if (tipoMovimento == 'E')
    {
        nuovoMovimento.tipologia = 'E';
    }
    else if (tipoMovimento == 'U')
    {
        nuovoMovimento.tipologia = 'U';
    }
    else
    {
        printf("Movimento non valido");
        return;
    }
    // setta l'importo del movimento
    nuovoMovimento.importo = importo;
    if (data < 367 && data > 0)
    {
        nuovoMovimento.data = data;
    }
    else
    {
        printf("Data non valida");
        return;
    }
    strcpy(nuovoMovimento.motivazione, motivazione);
    motivazione[strcspn(motivazione, "\n")] = '\0';

    return nuovoMovimento;
}
void anzianitaUtenti(struct utente utenti[])
{
    int min_eta = utenti[0].eta;
    int max_eta = utenti[0].eta;

    // Trova l'età minima e massima tra gli utenti
    for (int i = 1; i < NMAXUTENTI; i++)
    {
        if (utenti[i].eta < min_eta)
        {
            min_eta = utenti[i].eta;
        }
        if (utenti[i].eta > max_eta)
        {
            max_eta = utenti[i].eta;
        }
    }

    // Calcola il numero di utenti per ciascuna età compresa tra min_eta e max_eta
    int *conteggiEta = malloc((max_eta - min_eta + 1) * sizeof(int));
    for (int i = 0; i < max_eta - min_eta + 1; i++)
    {
        conteggiEta[i] = 0;
    }
    for (int i = 0; i < NMAXUTENTI; i++)
    {
        conteggiEta[utenti[i].eta - min_eta]++;
    }

    // Stampa i conteggi per età
    printf("Conteggi per età:\n");
    for (int i = 0; i < max_eta - min_eta + 1; i++)
    {
        printf("%d anni: %d\n", i + min_eta, conteggiEta[i]);
    }

    free(conteggiEta);
}
void inserisciNuovoUtente(struct utente elencoUtenti[], int *numUtenti)
{
    struct utente nuovoUtente;
    printf("Inserisci i dati del nuovo utente:\n");
    printf("Nome: ");
    scanf("%s", &nuovoUtente.nome);
    convertiInMaiuscolo(nuovoUtente.nome);
    printf("Cognome: ");
    scanf("%s", &nuovoUtente.cognome);
    convertiInMaiuscolo(nuovoUtente.cognome);

    printf("Codice fiscale: ");
    scanf("%s", &nuovoUtente.codice_fiscale);
    convertiInMaiuscolo(nuovoUtente.codice_fiscale);

    printf("Età: ");
    scanf("%d", &nuovoUtente.eta);
    printf("Città di residenza: ");
    scanf("%s", &nuovoUtente.citta_residenza);
    convertiInMaiuscolo(nuovoUtente.citta_residenza);

    printf("Provincia di residenza: ");
    scanf("%s", &nuovoUtente.provincia_residenza);
    convertiInMaiuscolo(nuovoUtente.provincia_residenza);

    nuovoUtente.nrMovimenti = 0; // il nuovo utente non ha ancora effettuato movimenti

    // controlla se il codice fiscale del nuovo utente è già presente
    for (int i = 0; i < *numUtenti; i++)
    {
        if (strcmp(nuovoUtente.codice_fiscale, elencoUtenti[i].codice_fiscale) == 0)
        {
            printf("Errore: codice fiscale già presente.\n");
            return; // esce dalla funzione senza inserire il nuovo utente
        }
    }

    // se il codice fiscale non è presente, inserisce il nuovo utente in fondo all'array
    elencoUtenti[*numUtenti] = nuovoUtente;

    // aggiorna il file utenti.txt con il nuovo utente
    FILE *fp = fopen("utenti.txt", "a");
    if (fp == NULL)
    {
        printf("Errore: impossibile aprire il file utenti.txt\n");
        return;
    }
    fprintf(fp, "%s %s %s %s %s %d\n", nuovoUtente.nome, nuovoUtente.cognome,
            nuovoUtente.codice_fiscale, nuovoUtente.citta_residenza,
            nuovoUtente.provincia_residenza, nuovoUtente.eta);
    fclose(fp);

    printf("Nuovo utente inserito con successo!\n");
}

int main()
{
    int scelta;
    struct utente elenco[NMAXUTENTI];
    int nUtenti = caricaUtenti("utenti.txt", elenco);
    char input[10];
    do
    {
        fflush(stdin);
        printf("\nMENU:\n");
        printf("1) Stampa elenco utenti e movimenti\n");
        printf("2) Calcola saldo di tutti gli utenti\n");
        printf("3) Statistiche personali in un periodo\n");
        printf("4) Inserisci nuovo movimento\n");
        printf("5) Statistiche su anzianità utenti\n");
        printf("6) Inserimento nuovo utente\n");
        printf("7) Esci\n");
        printf("Scelta: ");
        fgets(input, 10, stdin);

        // Rimuovi il carattere newline
        input[strcspn(input, "\n")] = '\0';

        // Conversione della stringa in un numero intero
        scelta = atoi(input);
        switch (scelta)
        {
        case 1:
            printf("\n--- ELENCO UTENTI E MOVIMENTI ---\n\n");
            stampaElencoUtenti(elenco);
            break;

        case 2:
            printf("\n--- CALCOLO SALDO UTENTI ---\n\n");
            stampaSaldoUtenti(elenco);
            break;

        case 3:
            printf("\n--- STATISTICHE PERSONALI IN UN PERIODO ---\n\n");
            int giornoMin, giornoMax;
            scanf("%s %d %d", cf, &giornoMin, &giornoMax);
            statisticheUtente(cf, elenco, giornoMin, giornoMax);
            break;

        case 4:
            printf("\n--- INSERIMENTO NUOVO MOVIMENTO ---\n\n");
            printf("Inserisci codice fiscale: ");
            scanf("%s", cf);
            scriviMovimentoSuFile("movimenti.txt", aggiungiMovimento(cercaUtente(cf, elenco)));
            printf("Movimento aggiunto con successo\n");
            break;

        case 5:
            printf("\n--- STATISTICHE SU ANZIANITA' UTENTI ---\n\n");
            anzianitaUtenti(elenco);
            break;

        case 6:
            printf("\n--- INSERIMENTO NUOVO UTENTE ---\n\n");
            inserisciNuovoUtente(elenco, &nUtenti);
            break;

        case 7:
            printf("\nArrivederci!\n");
            break;

        default:
            printf("\nScelta non valida, riprova.\n");
        }
    } while (scelta != 7);

    return 0;
}
