### Capteurs environnementaux et contrôleur d'arrosage sur batterie

#### Capteurs
Ce projet mesure la luminosité ambiante via un capteur [VEML7700](https://learn.adafruit.com/adafruit-veml7700) d'Adafruit, dans le but de contrôler certains équipements de la maison en fonction de la luminosité.
Il mesure également la pression atmosphérique via un capteur [MPL3115A2](https://www.adafruit.com/product/1893) ((température, altitude baromètre) surtout dans le but de faire des relevés, et pourquoi pas avertir de la probabilité d'un orage l'été.
Il y a également un capteur d'humidité du sol qui détermine si un arrosage est utile ou non.

#### Commandes
Ce projet peut commander un système d'arrosage via un relais. Le but étant d'arroser pendant une durée déterminée à intervalles réguliers.
Il doit prendre en compte l'humidité du sol pour éviter un arrosage inutile.

#### Alimentation
Ce projet étant éloigné du réseau Wifi de la maison, il doit pouvoir tourner sur batterie, il est donc relié à un circuit [TPL5110](https://learn.adafruit.com/adafruit-tpl5110-power-timer-breakout/usage) pour diminuer la consommation facilement. Le circuit réveille à intervalle régulier tout le projet, et lorsque les mesures et contrôles sont terminés le microcontrôleur envoie un signal d'extinction au circuit.
Tout le projet est alimenté en 3.3V via un petit convertisseur, pour stabiliser le courant de la batterie (qui sera une batterie au lithium) et pour éviter de détruire l'émetteur radio qui n'accepte pas 5V.

#### Communication
Toutes les données mesurées sont envoyées via un émetteur / récepteur 2.4Ghz NRF24l01 au serveur proche de la maison, en s'assurant que les données sont bien transmises (les erreurs de transmissions via mes émetteurs radio semblaient fréquentes dans mes tests).