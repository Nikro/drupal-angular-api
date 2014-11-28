#Drupal + Angular

*Okay, so more I thought about it, it would be nice to have 3 different modules:*

* **Angular API** - this offers AngularJS integration, libraries (CDN, Production, etc.), different useful tools and functions.

Project URL: https://github.com/Nikro/drupal-angular-api

* **Angular REST** - this offers a link between AngularJS and Services RESTful module. Basically just a consumption of services by $resource, but it would be awesome if we offer extra resources, specifically PER bundle, comment bundle, taxonomy bundle (term in voc), also write-up JS that automatically offers those services dynamically to be available.
Project URL: https://github.com/Nikro/drupal-angular-restful 

* **Angular Ctools** - this offers a link between AngularJS & Ctools. I was thinking a lot about this module, it can’t offer a custom content-type, because those will be basically offered by custom modules themselves. What it can offer, is some helper tools for easier work with AngularJS Apps and Controllers in the context of Ctools / Panels.
Project URL: https://github.com/Nikro/drupal-angular-ctools
