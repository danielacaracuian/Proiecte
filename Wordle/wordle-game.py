import pygame
import random

pygame.init()

# dimensiunile ferestrei jocului
width = 800
height = 700

# setam culorile
white = (255, 255, 255)
black = (0, 0, 0)
green = (30, 179, 0)
yellow = (255, 173, 1)
red = (255, 0, 0)

# aici setam fereastra jocului
screen = pygame.display.set_mode((width, height))
pygame.display.set_caption("Wordle Game")

# se coloreaza ecranul in alb
screen.fill(white)

# setam fonturile jocului
game_font = pygame.font.SysFont("arial", 60)
letter_font = pygame.font.SysFont("arial", 50)
message_font = pygame.font.SysFont("arial", 25)

# modelam titlul jocului
title = "Wordle Game"
title_text = game_font.render(title, True, black)
title_rect = title_text.get_rect(center = (width // 2, title_text.get_height() // 2 + 10))
# afisam titlul
screen.blit(title_text, title_rect)

# vectorul de cuvinte care se pot genera random (se mai adauga cuvinte)
words = ['ARIPA', 'ALBIE', 'ANEXA', 'ARICI', 'AROMA', 'BACIL', 'BATON', 'BOIER', 'BRIZA', 'BUTUC',
        'CAMIN', 'CIUMA', 'CHIST', 'COCOS', 'CAFEA', 'DAFIN', 'DIODA', 'DOLIU', 'DAUNA', 'DINTE',
        'EPOCA', 'EFECT', 'EMAIL', 'ETNIE', 'ETICA', 'FENOL', 'FOTON', 'FIOLA', 'FOLIE', 'FLIRT',
        'GLONT', 'GAZON', 'GAINA', 'GAMBA', 'GREVA', 'HARPA', 'HALVA', 'HUMUS', 'HULUB', 'HUSTE',
        'IARBA', 'IAURT', 'IARNA', 'INIMA', 'INGER', 'LACAS', 'LEMNE', 'LIMBA', 'LIPIE', 'LIMAN',
        'MAFIE', 'MANGO', 'MIXER', 'MODEL', 'MUZEU', 'NOMAD', 'NURSA', 'NOROI', 'NOTAR', 'NISIP',
        'ODAIE', 'OPERA', 'OASTE', 'OMIDA', 'OFTAT', 'PALMA', 'PANOU', 'PARIU', 'POZNA', 'PROBA',
        'REBUS', 'ROADA', 'REGIE', 'RICIN', 'RUBIN', 'SABIE', 'SARUT', 'SCAUN', 'SEARA', 'SEISM',
        'TUNET', 'TARAM', 'TURMA', 'TUTUN', 'TOPOR', 'URLET', 'URMAS', 'UNIRE', 'UNCHI', 'UMBRA',
        'VAPOR', 'VAMES', 'VIATA', 'VULPE', 'VREME', 'ZAHAR', 'ZIGOT', 'ZEFIR', 'ZEBRA', 'ZODIE']
# se alege random un cuvant
guessed_word = random.choice(words)

# dam print la cuvant ca sa l ghicim sa nu fim loseri
print(guessed_word)

# vector in care retinem toate cuvintele introduse de jucator
guesses = [[' ' for _ in range(5)] for _ in range(6)]
# cuvantul introdus de jucator
current_guess = ""

# functie pentru a desena tabla de joc
def draw_grid ():
    # setam dimensiunea unei casute
    box_size = 70

    # centram tabla de joc
    grid_width = 5 * box_size + 5 * 10 # latimea totala a tablei
    start_x = (width - grid_width) // 2 # pozitia de start pentru centrarea orizontala a tablei
    start_y = title_rect.bottom + 50 # pozitia de start pentru centrarea verticala a tablei
    
    # desenearea casutelor
    for row in range(6):
        for col in range(5):
            rect = pygame.Rect(start_x + col * (box_size + 10), start_y + row * (box_size + 10), box_size, box_size)
            pygame.draw.rect(screen, black, rect, 2)

            # aici afisam literele din vectorul de cuvinte in casute
            text = guesses[row][col]
            text_surface = letter_font.render(text, True, black)
            text_rect = text_surface.get_rect(center=rect.center)
            screen.blit(text_surface, text_rect)

    pygame.display.update()

# functie pentru printarea mesajelor corespunzatoare situatie in care se afla jocul
def print_messages (row, col):
    global current_guess

    if col < 5:
        message = "NOT ENOUGH LETTERS"
        message_text = message_font.render(message, True, yellow)
        message_rect = message_text.get_rect(center=(width // 2, height - 50))
        screen.blit(message_text, message_rect)
    elif col == 5:
        if guessed_word == current_guess.upper():
            message = "WHAT A GLORIOUS VICTORY! CONGRATS bruh!"
            message_text = message_font.render(message, True, green)
            message_rect = message_text.get_rect(center=(width // 2, height - 50))
            screen.blit(message_text, message_rect)
        elif row == 5:
            message = "WHAT A LOSER LMAO"
            message_text = message_font.render(message, True, red)
            message_rect = message_text.get_rect(center=(width // 2, height - 50))
            screen.blit(message_text, message_rect)
        else:
            # se verifica daca cuvantul introdus se contine in wordlist-ul jocului
            if current_guess.upper() in words:   
                message = "INCORRECT WORD"
                # se reseteaza datele pentru a introduce un nou cuvant
                row += 1
                col = 0
                # se reseteaza cuvantul
                current_guess = ""
            else:
                message = "INVALID WORD"
            message_text = message_font.render(message, True, black)
            message_rect = message_text.get_rect(center=(width // 2, height - 50))
            screen.blit(message_text, message_rect)

    pygame.display.update()
    return row, col

# se apeleaza functia de desenare a tablei de joc
draw_grid()

row = 0 # numarul cuvantului
col = 0 # indexul literei din cuvant
freeze = False # in cazul in care am introdus un cuvant (5 litere), nu permite introducerea unui alt cuvant pana nu l verific pe precedentul (pana nu apas ENTER)

# loop ul jocului
running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        elif event.type == pygame.KEYDOWN:
            # se verifica daca s a apasat tasta ENTER
            if event.key == pygame.K_RETURN:
                # se "deblocheaza" ecranul; se afiseaza mesajul corespunzator
                freeze = False
                row, col = print_messages(row, col)
            # se verifica daca tasta apasata este DELETE/BACKSPACE
            elif event.key == pygame.K_BACKSPACE:
                # se da unfreeze la ecran pentru a putea introduce alte caractere in locul celor sterse
                if col == 5:
                    freeze = False

                # se verifica daca pot sa dau delete la litera (daca nu s cumva fix pe prima casuta)
                if col > 0:
                    # se da delete la litera atat din cuvant cat si din vectorul de guesses
                    guesses[row][col - 1] = ""
                    current_guess = current_guess[: -1]
                    col -= 1

                # se afiseaza pe ecran cuvantul
                screen.fill(white)
                screen.blit(title_text, title_rect)
                draw_grid()
            # daca se apasa orice alta tasta de pe tastatura
            elif freeze == False:
                # se verifica ca tasta apasata sa fie litera
                if event.unicode.isalpha():
                    # obtinem litera apasata de la tastatura
                    letter = pygame.key.name(event.key)
                    
                    # se verifica daca am un cuvant format din 5 litere
                    if col < 5:
                        # se adauga litera la cuvant
                        current_guess += letter
                        # adaugam litera si in vectorul de cuvinte
                        guesses[row][col] = letter
                        # ne mutam la coloana urmatoare
                        col += 1

                        # daca au fost introduse 5 litere -> se ingheata ecranul pana la apasarea tastei ENTER
                        if col == 5:
                            freeze = True

                        # se afiseaza pe ecran cuvantul
                        screen.fill(white)
                        screen.blit(title_text, title_rect)
                        draw_grid()

pygame.quit()  
