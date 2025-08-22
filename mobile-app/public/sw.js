const CACHE_NAME = 'castrilha-v1';
const urlsToCache = [
  '/',
  '/index.html',
  '/static/js/bundle.js',
  '/static/css/main.css',
  '/assets/glasses-logo.png',
  '/assets/glasses-logo-small.png',
  '/assets/nomepx.png',
  '/assets/sounds/click.mp3',
  '/assets/sounds/notification.mp3',
  '/assets/sounds/success.mp3',
  '/assets/sounds/error.mp3',
  '/assets/sounds/connect.mp3',
  '/assets/sounds/disconnect.mp3',
  '/assets/sounds/navigation.mp3',
  '/assets/sounds/instruction.mp3',
  '/assets/sounds/hover.mp3',
  '/assets/sounds/toggle.mp3'
];

// Instalação do Service Worker
self.addEventListener('install', (event) => {
  event.waitUntil(
    caches.open(CACHE_NAME)
      .then((cache) => {
        console.log('Cache aberto');
        return cache.addAll(urlsToCache);
      })
      .catch((error) => {
        console.log('Erro ao cachear:', error);
      })
  );
});

// Ativação do Service Worker
self.addEventListener('activate', (event) => {
  event.waitUntil(
    caches.keys().then((cacheNames) => {
      return Promise.all(
        cacheNames.map((cacheName) => {
          if (cacheName !== CACHE_NAME) {
            console.log('Deletando cache antigo:', cacheName);
            return caches.delete(cacheName);
          }
        })
      );
    })
  );
});

// Interceptação de requisições
self.addEventListener('fetch', (event) => {
  event.respondWith(
    caches.match(event.request)
      .then((response) => {
        // Retorna do cache se encontrado
        if (response) {
          return response;
        }

        // Se não estiver no cache, busca da rede
        return fetch(event.request)
          .then((response) => {
            // Verifica se a resposta é válida
            if (!response || response.status !== 200 || response.type !== 'basic') {
              return response;
            }

            // Clona a resposta para cache
            const responseToCache = response.clone();

            caches.open(CACHE_NAME)
              .then((cache) => {
                cache.put(event.request, responseToCache);
              });

            return response;
          })
          .catch(() => {
            // Se offline e não estiver no cache, retorna página offline
            if (event.request.destination === 'document') {
              return caches.match('/index.html');
            }
          });
      })
  );
});

// Sincronização em background
self.addEventListener('sync', (event) => {
  if (event.tag === 'background-sync') {
    event.waitUntil(doBackgroundSync());
  }
});

// Notificações push
self.addEventListener('push', (event) => {
  const options = {
    body: event.data ? event.data.text() : 'Nova notificação do Castrilha',
    icon: '/assets/glasses-logo-small.png',
    badge: '/assets/glasses-logo-small.png',
    vibrate: [100, 50, 100],
    data: {
      dateOfArrival: Date.now(),
      primaryKey: 1
    },
    actions: [
      {
        action: 'explore',
        title: 'Abrir App',
        icon: '/assets/glasses-logo-small.png'
      },
      {
        action: 'close',
        title: 'Fechar',
        icon: '/assets/glasses-logo-small.png'
      }
    ]
  };

  event.waitUntil(
    self.registration.showNotification('Castrilha', options)
  );
});

// Clique em notificação
self.addEventListener('notificationclick', (event) => {
  event.notification.close();

  if (event.action === 'explore') {
    event.waitUntil(
      clients.openWindow('/')
    );
  }
});

// Função para sincronização em background
function doBackgroundSync() {
  // Aqui você pode implementar sincronização de dados
  console.log('Sincronização em background executada');
  return Promise.resolve();
} 