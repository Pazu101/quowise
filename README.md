# **À quoi ça sert ?**

Quowise s'inspire de Readwise (service auquel j'ai été client quelques années) qui me permettait d'enregistrer des citations depuis mon téléphone, kindle ou mes lectures sur internet et Readwise me les envoyait par mail et/ou notification à un rythme défini dans mes paramètres (leur service à abonnement propose foule d'autres features).

## **Pourquoi ?**
Pour m'entraîner sur un truc que je vais utiliser pour de vrai

## **Comment ?**
Le programme est développé en C parce que pourquoi pas.
J'utilise **sqlite3** pour enregistrer les citations, **libcurl** pour envoyer un email via SMTP et un script bash pour le lancer automatiquement (tous les trois jours par exemple) et rediriger l'output dans des fichiers .log.

Me renvoyer des citations c'est bien mais si j'ai trois fois la même c'est chiant, alors j'applique de la **pondération** :
- Lorsqu'une nouvelle entrée arrive dans la base de données elle a un poids de 10.
- Le programme fait le total de tous les poids cumulés, le *total_shares* qui va représenter notre gâteau complet (chaque citation ayant une part plus ou moins grande de ce gros gâteau)
- Il génère en suite un nombre aléatoire au modulo des *total_shares* et additionne à la suite tous les poids en comparant à chaque fois le *cumulative_weight* (incrémenté à chaque itération de la boucle) au nombre aléatoire.
- Dès que le nombre aléatoire est dépassé, la citation est sélectionnée et son ID retenu pour être passé aux autres fonctions.

J'ai un peu galéré avec le fonctionnement de libcurl et le protocole SMTP mais en gros de ce que j'ai compris :
- On utilise du SSL pour sécuriser la connexion et la fonction VRFY est désactivée par le code pour que certains serveurs (comme iCloud) ne bloque pas l'envoie des data.
- Les données transférées au mail s'appellent le *payload* et elles doivent suivre une mise en forme stricte à base de *\r* et de *snprintf* (en gros c'est une fonction qui print des données formatées avec une taille max et une destination précise qui retourne le nombre de charactères écrits dans la destination, sans le NULL).  
- C'est tout et c'est déjà pas mal.

# Trop cool ton programme, je peux l'utiliser ?
Merci c'est trop sympa. Tu peux participer, utiliser ou modifier mon projet comme tu veux en respectant la license [CC BY-NC-SA](https://creativecommons.org/licenses/by-nc-sa/4.0/).
Pour le faire marcher il est nécessaire d'avoir quelques fonctions que je n'ai pas push ici (principalement par flemme) donc n'hésite pas à me demander, pareil pour le script qui fait tourner le programme en solo. 

***Merci de m'avoir lu et à bientôt sur Github 🤖***
