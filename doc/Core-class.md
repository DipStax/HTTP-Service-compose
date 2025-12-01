# Core class of HSC

## Implementation

### `ServiceBuilder`

- The compile-time/configuration API you use while assembling your application.
- Collects service registrations, controller discovery info and route definitions.
- Validates DI constraints (singleton/scoped/transient rules) at registration/creation time where possible.

### `ServiceCollection`

- Runtime execution environment returned by ServiceBuilder::build().
- Owns the ServiceProvider and the middleware/route pipeline configuration.
- Provides APIs to add middleware and to dispatch incoming requests.

## Builtin

### Services

#### `ServiceProvider`

_Based on `AServiceProvider`_

- Acts as the single runtime source of services.
- Responsible for creating and returning service instances according to their lifetime rules.
- Manages shared singleton instances, per-request scoped instances, and on-demand transient instances.
- Exposes a simple API for retrieving services by their interface.

#### `RouteProvider`

_Based on `IRouteProvider`_

- Holds the list of available routes (path + method + handler).
- Matches incoming requests to a registered route and triggers the corresponding handler.
- Ensures a controller instance is created for the matched route and that the handler is executed with injected dependencies.

### Middlewares

#### `DispatchRoute`

- Final middleware in the pipeline.
- Receives the normalized request context and delegates routing to `IRouteProvider`.
- Bridges the generic middleware chain with the route dispatch mechanism.