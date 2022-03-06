import React from 'react';
import Home from './containers/home';
import Subscribers from './containers/subscribers';
import Networks from './containers/networks';
import Accounts from './containers/accounts';
import Speedtest from './containers/speedtest';
import SmallLogo from './assets/images/Group.png';
import {
  BrowserRouter as Router,
  Switch,
  Route,
  NavLink,
  Link
} from "react-router-dom";
import RouterTraffic from './containers/routerTraffic';
import IspPlans from './containers/IspPlans';
import ScriptLibrary from './containers/ScriptLibrary';
import SubscribersList from './containers/SubscribersList';
import Preferences from './containers/Preferences';
import SubscriberSingle from './containers/SubscriberSingle';

class App extends React.Component {
  render() {
    return (
      <Router>
        <div className="ui-layout">
          <div className="ui-sidebar">
            <div className="ui-logo-box">
              <div className="ui-logo-wrapper">
                <img className="ui-logo-small" alt="Small logo" src={SmallLogo} />
              </div>
              <div className="ui-logo-label">
                <p>Smart Network</p>
              </div>
            </div>
            <div className="ui-vertical-menu">
              <NavLink activeClassName="active" to="/subscribers"  >
                <img src={require("./assets/images/Shape-3.png")} alt="Subscribers icon" />
                <p>Subscribers</p>
              </NavLink>

              <NavLink activeClassName="active" to="/SubscribersList"  >
                <img src={require("./assets/images/Shape-3.png")} alt="Subscribers icon" />
                <p>Subscribers List</p>
              </NavLink>

              <NavLink activeClassName="active" to="/SubscriberSingle"  >
                <img src={require("./assets/images/Shape-3.png")} alt="Subscribers icon" />
                <p>Subscribers Single</p>
              </NavLink>


              <NavLink activeClassName="active" to="/networks" >
                <img src={require("./assets/images/Shape-4.png")} alt="Networks icon" />
                <p>Networks</p>
              </NavLink>
              <NavLink activeClassName="active" to="/ispPlans">
                <img src={require("./assets/images/Shape-5.png")} alt="ISP Plans icon" />
                <p>ISP Plans</p>
              </NavLink>
              <Link>
                <img src={require("./assets/images/Shape-2.png")} alt="Task manager icon" />
                <p>Task Manger</p>
              </Link>
              <NavLink activeClassName="active" to="/ScriptLibrary">
                <img src={require("./assets/images/Shape-1.png")} alt="Script library icon" />
                <p>Script Library</p>
              </NavLink>
              <NavLink activeClassName="active" to="/Preferences">
                <img src={require("./assets/images/Shape-6.png")} alt="Preferences icon" />
                <p>Preferences</p>
              </NavLink>
              <NavLink activeClassName="active" to="/speedtest" >
                <img src={require("./assets/images/Shape-4.png")} alt="Networks icon" />
                <p>Speedtest</p>
              </NavLink>
              <NavLink activeClassName="active" to="/routerTraffic" >
                <img src={require("./assets/images/Shape-4.png")} alt="Networks icon" />
                <p>Router Traffic</p>
              </NavLink>
              <div class="ui-vertical-menu">
                <p class="ui-menu-label">SYSTEM</p>
                <NavLink activeClassName="active" to="/accounts" >
                  <img src={require("./assets/images/user.png")} alt="Accounts icon" />
                  <p>Accounts</p>
                </NavLink>
                <Link>
                  <img src={require("./assets/images/Shape-7.png")} alt="Organizations icon" />
                  <p>Organizations</p>
                </Link>
                <Link>
                  <img src={require("./assets/images/Shape-8.png")} alt="Admins icon" />
                  <p>Admins</p>
                </Link>
                <Link>
                  <img src={require("./assets/images/Shape.png")} alt="Support icon" />
                  <p>Support</p>
                </Link>
              </div>
              <div class="ui-user">
                <img src={require("./assets/images/user.png")} alt="Avatar" />
                <p>ofer@valleyinternet.net</p>
              </div>
            </div>
          </div>
          <Switch>
            <Route path="/accounts">
              <Accounts />
            </Route>
            <Route path="/subscribers">
              <Subscribers />
            </Route>
            <Route path="/SubscribersList">
              <SubscribersList />
            </Route>
            <Route path="/SubscriberSingle">
              <SubscriberSingle />
            </Route>
            <Route path="/networks">
              <Networks />
            </Route>
            <Route path="/speedtest">
              <Speedtest />
            </Route>
            <Route path="/routerTraffic">
              <RouterTraffic />
            </Route>
            <Route path="/ispPlans">
              <IspPlans />
            </Route>
            <Route path="/ScriptLibrary">
              <ScriptLibrary />
            </Route>
            <Route path="/Preferences">
              <Preferences />
            </Route>
            <Route path="/">
              <Home />
            </Route>
          </Switch>
        </div>
      </Router>
    );
  }
}

export default App;
